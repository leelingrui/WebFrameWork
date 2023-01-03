#include "spin_lock.h"

namespace lock
{
	spin_lock::spin_lock() : flag(0)
	{
	}
	void spin_lock::acquire(size_t num)
	{
		size_t expected = 0;
		std::thread::id current_id = std::this_thread::get_id();
		lock_id();
		if (m_thread_id == current_id)
		{
			flag += num;
			unlock_id();
		}
		else
		{
			while (true)
			{
				unlock_id();
				lock_id();
				if (flag == 0)
				{
					flag = num;
					m_thread_id = current_id;
					unlock_id();
					break;
				}
			}
		}
	}

	void spin_lock::release(size_t num)
	{
		size_t expected = 0;
		lock_id();
		if (m_thread_id == std::this_thread::get_id())
		{
			flag -= num;
			unlock_id();
		}
		else
		{
			throw std::runtime_error("lock and release from different thread!");
		}
	}
	void spin_lock::lock_id()
	{
		while (lock_thread_id.test_and_set());
	}
	void spin_lock::unlock_id()
	{
		lock_thread_id.clear();
	}
}

