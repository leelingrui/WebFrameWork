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

namespace thread
{
	template<typename _Tp>
	struct Task 
	{
		struct TaskPromise {
			_Tp result;
			Task get_return_object() {
				return std::coroutine_handle<TaskPromise>::from_promise(*this);
			}
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			std::suspend_always yield_value(_Tp&& value)
			{
				result = std::forward<_Tp>(value);
				return {};
			}
			void return_void() {};
			void unhandled_exception() {}
		};

		using promise_type = TaskPromise;
		Task(std::coroutine_handle<TaskPromise>&& h) : handle(std::move(h)) {}
		Task(std::coroutine_handle<TaskPromise>& h) : handle(std::move(h)) {}
		inline bool finished() { return handle.done(); }
		inline _Tp GetResult()
		{
			return handle->promise().result;
		}
		std::coroutine_handle<TaskPromise> handle;
	};

	class threadPoolBase
	{
	public:
		threadPoolBase(unsigned short threads);
		virtual ~threadPoolBase();
		template<typename func, typename... Args>
		auto submit(func&& f, Args && ... args)
		{
			std::function<decltype(f(args...))()> task = std::bind(std::forward<func>(f), std::forward<Args>(args)...);
			std::shared_ptr<std::packaged_task<decltype(f(args...))()>> task_pointer = std::make_shared<std::packaged_task<decltype(f(args...))()>>(task);
			mut_objectPool.lock();
			std::function<void()>* warpper_func = objectPool.alloc();
			mut_objectPool.unlock();
			*warpper_func = [task_pointer]() ->void { (*task_pointer)(); };
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
	protected:
		virtual void worker();
		std::counting_semaphore<9223372036854775807> tasks{0};
		std::queue<std::function<void()>*> urgent, normal, running;
		memory::MemoryPool<std::function<void()>> objectPool;
		std::mutex mut_urgent, mut_normal, mut_running, mut_objectPool;
		std::vector<std::jthread> working_threads;
		bool Terminated = false;
		unsigned short threadnum;
	};
}
#endif