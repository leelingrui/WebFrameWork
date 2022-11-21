#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <thread>
#include <mutex>
#include <semaphore>
#include <queue>
#include <any>
#include <vector>
#include <memory>
#include <stdarg.h>
#include <MemoryPool.h>
#include <functional>
#include <future>
#include <coroutine>
#include <map>
#include <type_traits>
#include <traits.h>

namespace thread
{
	class threadPoolBase;
	template<typename _Tp>
	struct Task
	{
		struct suspend_task;
		struct TaskPromise {
			std::promise<_Tp> result;
			Task<_Tp> get_return_object() {
				return std::coroutine_handle<TaskPromise>::from_promise(*this);
			}
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			std::suspend_always yield_value(_Tp&& value)
			{
				result.set_value(std::forward<_Tp>(value));
				return {};
			}
			void return_value(_Tp value) { result.set_value(value); };
			void unhandled_exception() {};
		};


		struct suspend_task
		{
			threadPoolBase* m_threadpool;
			constexpr bool await_ready() const noexcept { return false; };
			constexpr void await_suspend(std::coroutine_handle<>&& h) const noexcept
			{
				std::function<void()> resume = [this, &h]() { 
					std::unique_lock<std::mutex>(this->m_threadpool->mut_running);
					std::unique_lock<std::mutex>(this->m_threadpool->mut_objectPool);
					std::function<void()>* wapper_func = this->m_threadpool->objectPool.alloc();
					*wapper_func = [&h]() -> void {	h.resume(); };
					this->m_threadpool->running.push(wapper_func);
					this->m_threadpool->tasks.release();
				};
				m_threadpool->mut_suspendMap.lock();
				m_threadpool->suspend_map[h] = resume;
				m_threadpool->mut_suspendMap.unlock();
			};
			constexpr void await_resume() const noexcept {};
		};

		using promise_type = TaskPromise;
		Task(std::coroutine_handle<promise_type>&& h) : handle(std::move(h)), m_future(h.promise().result.get_future()) {}
		Task(std::coroutine_handle<promise_type>& h) : handle(std::move(h)), m_future(h.promise().result.get_future()) {}
		Task(std::future<_Tp>&& future) : m_future(std::move(future)) {}
		Task(Task<_Tp>& t) : m_future(std::move(t.m_future)), handle(std::move(t.handle)) {}
		Task<_Tp>& operator=(Task<_Tp>&& t)
		{
			this->handle = std::move(t.handle);
			this->m_future = std::move(t.m_future);
			return *this;
		}
		static suspend_task suspend(threadPoolBase* thread_pool)
		{
			return suspend_task(thread_pool);
		}
		inline bool finished() { return handle.done(); }
		_Tp GetResult()
		{
			return m_future.get();
		}
		std::coroutine_handle<TaskPromise> handle;
		std::future<_Tp> m_future;
	};

	template<typename _Tp>
	concept IsTask = type_traits::is_base_template_of<Task, _Tp>::value;

	class threadPoolBase
	{
	public:
		threadPoolBase(unsigned short threads = 2);
		virtual ~threadPoolBase();
		template<typename _Ret, typename... Args>
		auto submit(_Ret(&&f)(Args...), Args && ... args)
		{
			if constexpr (type_traits::is_base_template_of<Task, _Ret>::value)
			{
				_Ret task = f(args...);
				mut_objectPool.lock();
				std::function<void()>* warpper_func = objectPool.alloc();
				mut_objectPool.unlock();
				*warpper_func = [&task]() -> void
				{
					task.handle.resume();
				};
				mut_normal.lock();
				normal.push(warpper_func);
				mut_normal.unlock();
				tasks.release();
				return task;
			}
			else 
			{
				std::function<decltype(f(args...))()> task = std::bind(std::forward<_Ret(Args...)>(f), std::forward<Args>(args)...);
				std::shared_ptr<std::packaged_task<decltype(f(args...))()>> task_pointer = std::make_shared<std::packaged_task<decltype(f(args...))()>>(task);
				mut_objectPool.lock();
				std::function<void()>* warpper_func = objectPool.alloc();
				mut_objectPool.unlock();
				*warpper_func = [task_pointer]() -> void { (*task_pointer)(); };
				mut_normal.lock();
				normal.push(warpper_func);
				mut_normal.unlock();
				tasks.release();
				return Task<_Ret>(task_pointer->get_future());
			}

		}
		template<typename _Ret, typename classname, typename... Args>
		auto submit(_Ret(classname::*f)(Args...), classname *c, Args && ... args)
		{
			std::function<_Ret(Args...)> task = std::bind(f, c, std::forward<Args>(args)...);
			std::shared_ptr<std::packaged_task<_Ret(Args...)>> task_pointer = std::make_shared<std::packaged_task<_Ret(Args...)>>(task);
			mut_objectPool.lock();
			std::function<void()>* warpper_func = objectPool.alloc();
			mut_objectPool.unlock();
			*warpper_func = [task_pointer]() -> void { (*task_pointer)(); };
			mut_normal.lock();
			normal.push(warpper_func);
			mut_normal.unlock();
			tasks.release();
			return task_pointer->get_future();
		}

		template<typename func, typename... Args>
		auto urgentSubmit(const func&& f, const Args && ... args)
		{
			std::function<decltype(f(args...))()> task = std::bind(std::forward<func>(f), std::forward<Args>(args)...);
			std::shared_ptr<std::packaged_task<decltype(f(args...))()>> task_pointer = std::make_shared<std::packaged_task<decltype(f(args...))()>>(task);
			mut_objectPool.lock();
			std::function<void()>* warpper_func = objectPool.alloc();
			mut_objectPool.unlock();
			*warpper_func = [task_pointer]() ->void { (*task_pointer)(); };
			mut_urgent.lock();
			urgent.push(warpper_func);
			mut_urgent.unlock();
			tasks.release();
			return task_pointer->get_future();
		}

		template<IsTask _Tp>
		void resume(_Tp& task)
		{
			std::unique_lock<std::mutex> lock(mut_suspendMap);
			std::map<std::coroutine_handle<>, std::function<void()>>::iterator&& iter = suspend_map.find(task.handle);
			if (iter != suspend_map.end())
			{
				(iter->second)();
				suspend_map.erase(iter);
			}
		}
	protected:
		virtual void worker();
		std::counting_semaphore<9223372036854775807> tasks{0};
		std::queue<std::function<void()>*> urgent, normal, running;
		std::map<std::coroutine_handle<>, std::function<void()>> suspend_map;
		memory::MemoryPool<std::function<void()>> objectPool;
		std::mutex mut_urgent, mut_normal, mut_running, mut_objectPool, mut_suspendMap;
		std::vector<std::jthread> working_threads;
		bool Terminated = false;
		unsigned short threadnum;
		template<typename _Tp> friend struct Task;
	};
}
#endif