#include <ThreadPool.h>

namespace thread
{
	threadPoolBase::threadPoolBase(unsigned short threads)
	{
		threadnum = threads;
		for (int var = 0; var < threads; var++)
		{
			working_threads.emplace_back(std::jthread(&threadPoolBase::worker, this));
			//working_threads[var].detach();
		}
	}

	threadPoolBase::~threadPoolBase()
	{
		Terminated = true;
		tasks.release(threadnum);
		for (int i = 0; i < working_threads.size(); i++)
		{
			if (working_threads[i].joinable()) // �ж��߳��Ƿ��ڵȴ�
			{
				working_threads[i].join(); // ���̼߳��뵽�ȴ�����
			}
		}
		while (!running.empty())
		{
			objectPool.free(std::move(running.front()));
			running.pop();
		}
		while (!normal.empty())
		{
			objectPool.free(std::move(normal.front()));
			normal.pop();
		}
		while (!urgent.empty())
		{
			objectPool.free(std::move(urgent.front()));
			urgent.pop();
		}
	}

	void threadPoolBase::worker()
	{
		std::function<void()>* context;
		while (!Terminated)
		{
			tasks.acquire();
			context = nullptr;
			if (!running.empty())
			{
				mut_running.lock();
				if (!running.empty())
				{
					context = std::move(running.front());
					running.pop();
				}
				mut_running.unlock();
			}
			if (!urgent.empty())
			{
				mut_urgent.lock();
				if (!urgent.empty())
				{
					context = std::move(urgent.front());
					urgent.pop();
				}
				mut_urgent.unlock();
			}
			if (!normal.empty())
			{
				mut_normal.lock();
				if (!normal.empty())
				{
					context = std::move(normal.front());
					normal.pop();
				}
				mut_normal.unlock();
			}
			if (context)
			{
				(*context)();
				mut_objectPool.lock();
				objectPool.free(context);
				mut_objectPool.unlock();
			}
		}
	}
}