#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <mutex>

namespace lock
{
	class spin_lock
	{
	public:
		spin_lock();
		void acquire(size_t num = 1);
		void release(size_t num = 1);
	private:
		inline void lock_id();
		inline void unlock_id();
		size_t flag;
		std::atomic_flag lock_thread_id = ATOMIC_FLAG_INIT;
		std::thread::id m_thread_id;
	};
}

#endif