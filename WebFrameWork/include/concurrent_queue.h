#ifndef COUNCURRENT_QUEUE_H
#define COUNCURRENT_QUEUE_H
#include <queue>
#include <mutex>
#include <spin_lock.h>
#include <vector>
namespace councurrency
{
	template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
	class councurrent_queue
	{
	public:
		void enqueue(_Tp&& item)
		{
			std::deque<int> vec;
			std::allocator<int> alloc;
			alloc.deallocate
			vec.emplace;
			_Alloc.allocate();
		}
		bool try_dequeue(_Tp& item);
	private:
		typedef struct ListNode
		{
			struct ListNode* next;
			_Tp value;
		} LIST;
		lock::spin_lock global_lock;
		std::queue<_Tp> queue;
	};
}
#elif