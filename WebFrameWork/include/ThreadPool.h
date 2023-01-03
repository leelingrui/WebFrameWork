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
#include <shared_mutex>
#include <set>
#include <map>
#include <spin_lock.h>
#include <type_traits>
#include <traits.h>

namespace thread
{
	class threadPoolBase;
	using threadPool = threadPoolBase;
	class awaitable
	{
		bool await_ready()noexcept { return false; };
		void await_suspend(std::coroutine_handle<>&& h) noexcept;
	};
	template<typename _Tp> struct suspend_task;

	template<typename _Tp>
	struct Task
	{
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

		using promise_type = TaskPromise;
		Task(std::coroutine_handle<promise_type>&& h) : m_future(h.promise().result.get_future())
		{
			handle.reset(new std::coroutine_handle<promise_type>);
			*handle = std::move(h);
		}
		Task(std::coroutine_handle<promise_type>& h) : m_future(h.promise().result.get_future())
		{
			handle.reset(new std::coroutine_handle<promise_type>);
			*handle = std::move(h);
		}
		Task(std::future<_Tp>&& future) : m_future(std::move(future)) {}
		Task(Task<_Tp>& t) : m_future(std::move(t.m_future)), handle(t.handle) {}
		Task<_Tp>& operator=(Task<_Tp>&& t)
		{
			this->handle = std::move(t.handle);
			this->m_future = std::move(t.m_future);
			return *this;
		}
		inline bool finished() { return handle.done(); }
		_Tp GetResult()
		{
			return m_future.get();
		}
		std::shared_ptr<std::coroutine_handle<promise_type>> handle;
		std::future<_Tp> m_future;
	};
	template<typename _Tp>
	struct suspend_task : public awaitable
	{
		threadPoolBase* m_threadpool;
		_Tp value;
		suspend_task(threadPoolBase* base)
		{
			m_threadpool = base;
			value = 0;
		}
		constexpr bool await_ready() const noexcept { return false; };
		constexpr void await_suspend(std::coroutine_handle<>&& h) const noexcept
		{
			m_threadpool->mut_suspendMap.acquire();
			std::set<std::coroutine_handle<>>::iterator&& iter = m_threadpool->cancel_set.find(h);
			if (iter != m_threadpool->cancel_set.end())
			{
				m_threadpool->cancel_set.erase(h);
			}
			else
			{
				m_threadpool->suspend_map.insert(std::make_pair(h, const_cast<suspend_task<_Tp>*>(this)));
			}
			m_threadpool->mut_suspendMap.release();
		};
		void setValue(_Tp&& m_val)
		{
			value = m_val;
		}
		_Tp& await_resume() noexcept { return value; };
	};

	template<typename _Tp>
	concept IsTask = type_traits::is_base_template_of<Task, _Tp>::value;
	template<typename _Tp>
	concept IsAwaitable = std::is_base_of<awaitable, _Tp>::value;
	class threadPoolBase
	{
	public:
		threadPoolBase();
		threadPoolBase(unsigned short threads);
		virtual ~threadPoolBase();
		template<typename _Ret, typename... Args>
		auto submit(_Ret(&&f)(Args...), Args && ... args)
		{
			if constexpr (type_traits::is_base_template_of<Task, _Ret>::value)
			{
				_Ret task = f(args...);
				decltype(task.handle)& handle = task.handle;
				std::function<void()>* warpper_func = objectPool.alloc();
				*warpper_func = [handle]() -> void
				{
					handle->resume();
				};
				mut_normal.acquire();
				normal.push(warpper_func);
				mut_normal.release();
				tasks.release();
				return task;
			}
			else 
			{
				std::function<decltype(f(args...))()> task = std::bind(std::forward<_Ret(Args...)>(f), std::forward<Args>(args)...);
				std::shared_ptr<std::packaged_task<decltype(f(args...))()>> task_pointer = std::make_shared<std::packaged_task<decltype(f(args...))()>>(task);
				std::function<void()>* warpper_func = objectPool.alloc();
				*warpper_func = [task_pointer]() -> void { (*task_pointer)(); };
				mut_normal.acquire();
				normal.push(warpper_func);
				mut_normal.release();
				tasks.release();
				return Task<_Ret>(task_pointer->get_future());
			}

		}
		template<typename _Ret, typename classname, typename... Args>
		auto submit(_Ret(classname::*f)(Args...), classname *c, Args && ... args)
		{
			std::function<_Ret(Args...)> task = std::bind(f, c, std::forward<Args>(args)...);
			std::shared_ptr<std::packaged_task<_Ret(Args...)>> task_pointer = std::make_shared<std::packaged_task<_Ret(Args...)>>(task);
			std::function<void()>* warpper_func = objectPool.alloc();
			*warpper_func = [task_pointer]() -> void { (*task_pointer)(); };
			mut_normal.acquire();
			normal.push(warpper_func);
			mut_normal.release();
			tasks.release();
			return task_pointer->get_future();
		}

		template<typename func, typename... Args>
		auto urgentSubmit(const func&& f, const Args && ... args)
		{
			std::function<decltype(f(args...))()> task = std::bind(std::forward<func>(f), std::forward<Args>(args)...);
			std::shared_ptr<std::packaged_task<decltype(f(args...))()>> task_pointer = std::make_shared<std::packaged_task<decltype(f(args...))()>>(task);
			std::function<void()>* warpper_func = objectPool.alloc();
			*warpper_func = [task_pointer]() ->void { (*task_pointer)(); };
			mut_urgent.acquire();
			urgent.push(warpper_func);
			mut_urgent.release();
			tasks.release();
			return task_pointer->get_future();
		}

		template<IsTask _Tp>
		bool resume(_Tp& task)
		{
			std::unique_lock<std::mutex> lock(mut_suspendMap);
			std::map<std::coroutine_handle<>, awaitable*>::iterator&& iter = suspend_map.find(*task.handle);
			if (iter != suspend_map.end())
			{
				std::function<void()>* warpper_func = objectPool.alloc();
				decltype(task.handle)& handle = task.handle;
				*warpper_func = [handle]() -> void
				{
					handle->resume();
				};
				mut_running.acquire();
				running.push(warpper_func);
				mut_running.release();
				suspend_map.erase(iter);
				tasks.release();
				return true;
			}
			else return false;
		}

		template<IsTask _Tp>
		bool cancel(_Tp& task)
		{
			mut_suspendMap.acquire();
			std::map<std::coroutine_handle<>, awaitable*>::iterator&& iter = suspend_map.find(*task.handle);
			if (iter != suspend_map.end())
			{
				suspend_map.erase(iter);
			}
			else
			{
				cancel_set.insert(*task.handle);
			}
			mut_suspendMap.release();
		}

		template<template<typename ...Args> typename _Await, typename _Tp, typename ...Args>
		bool resume(_Tp& task, Args&&... val)
		{
			mut_suspendMap.acquire();
			std::map<std::coroutine_handle<>, awaitable*>::iterator&& iter = suspend_map.find(*task.handle);
			if (iter != suspend_map.end())
			{
				std::function<void()>* warpper_func = objectPool.alloc();
				auto a = iter->second;
				static_cast<_Await<Args...>*>(iter->second)->setValue(std::move(val...));
				decltype(task.handle)& handle = task.handle;
				*warpper_func = [handle]() -> void
				{
					handle->resume();
				};
				mut_running.acquire();
				running.push(warpper_func);
				mut_running.release();
				suspend_map.erase(iter);
				tasks.release();
				mut_suspendMap.release();
				return true;
			}
			else
			{
				mut_suspendMap.release();
				return false;
			}
		}
	protected:
		virtual void worker();
		std::counting_semaphore<100> tasks{0};
		std::queue<std::function<void()>*> urgent, normal, running;
		std::map<std::coroutine_handle<>, awaitable*> suspend_map;
		std::set<std::coroutine_handle<>> cancel_set;
		memory::MemoryPool<std::function<void()>> objectPool;
		lock::spin_lock mut_urgent, mut_normal, mut_running, mut_suspendMap;
		std::vector<std::jthread> working_threads;
		bool Terminated = false;
		unsigned short threadnum;
		template<typename _Tp> friend struct suspend_task;
	};
}
#endif