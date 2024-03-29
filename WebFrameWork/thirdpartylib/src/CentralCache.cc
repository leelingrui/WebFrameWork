#define _CRT_SECURE_NO_WARNINGS 1
#include "CentralCache.h"
#include "PageCache.h"

CentralCache CentralCache::_cc;


Span* CentralCache::GetOneSpan(SpanList& list, size_t bytes) // 假如Span为空，获取Span
{
	// 先遍历链表看看有无span，有就拿
	Span* it = list.Begin();
	while (it != list.End())
	{
		if (it->_SpanFreeList != nullptr) // 有就拿走
		{
			return it;
		}
		else // 没有就下一个
		{
			it = it->_next;
		}
	}

	// 要先解开桶锁
#pragma warning(push)
#pragma warning(disable:26110)
	list.CC_mtx.unlock();
#pragma warning(pop)
	// 走到这说明CC没有，开始问PC拿span，申请一块大内存，无组装过，纯净的，拿了需要组装，不过得加锁处理
	PageCache::GetInstance()->PC_mtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(bytes));
	span->_IsUse = true;
	span->_objectSize = bytes;
	PageCache::GetInstance()->PC_mtx.unlock();

	// 起始位置
	char* start = (char*)(span->_pageId << PAGESHIFT);
	size_t BigBytes = span->_n << PAGESHIFT; // 页数*8k
	char* end = start + BigBytes;  // 加上整个就算数组大小
	// 开始组装，把一块大内存组装成一个自由链表

	span->_SpanFreeList = start;
	start += bytes;
	void* tail = span->_SpanFreeList;
	//int i = 1;
	while (start < end)
	{
		NextObj(tail) = start;
		tail = start;
		start += bytes;
		//i++;
	}
	//cout << i << endl;

	NextObj(tail) = nullptr;

	//size_t j = 0;
	//void* cur = span->_SpanFreeList;
	//while (cur)
	//{
	//	cur = NextObj(cur);
	//	j++;
	//}
	//if (j != BigBytes / bytes)
	//{
	//	int x = 0;
	//}

	// 头插进CC的byte上
	list.CC_mtx.lock();
	list.PushFront(span);

	return span;
}


size_t CentralCache::GetRangeObj(void*& start, void*& end, size_t batchNum, size_t bytes) // 实际给TC的数量
{
	size_t index = SizeClass::Index(bytes);
	_CCSpanList[index].CC_mtx.lock();
	// TODO
	Span* span = GetOneSpan(_CCSpanList[index], bytes); // 要拿内存，至少这个节点上有才行
	assert(span);
	assert(span->_SpanFreeList);

	start = span->_SpanFreeList;
	end = start; // end先等于start，然后再往后跑

	size_t i = 0, actualNum = 1;
	while (i < batchNum-1 && NextObj(end) != nullptr) // -1 是因为end
	{
		end = NextObj(end); // 往后走
		i++;
		actualNum++;
	}
	span->_SpanFreeList = NextObj(end); // 拿走后把剩下的第一个连上
	NextObj(end) = nullptr; // 把拿走的最后一个的_next置空

	span->_usecount += actualNum;  // 拿走几个就+几个
	// ..还不够怎么办
	
	//size_t j = 0;
	//void* cur = start;
	//while (cur)
	//{
	//	cur = NextObj(cur);
	//	j++;
	//}
	//if (j != actualNum)
	//{
	//	int x = 0;
	//}

	_CCSpanList[index].CC_mtx.unlock(); // 解锁

	return actualNum;
}

void CentralCache::ReleaseListToSpan(void* start, size_t bytes) // 处理从TC回收的自由链表
{
	size_t index = SizeClass::Index(bytes);
	
	// 回收CC[index]上的哪一个span呢？用MapObjectToSpan找到哪一页上的span
	_CCSpanList[index].CC_mtx.lock();
	while (start)
	{
		void* next = NextObj(start);
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start); // 找到对应映射的span上
		// 头插,双向
		NextObj(start) = span->_SpanFreeList;
		span->_SpanFreeList = start;
		span->_usecount--; // 归还减一

		// usecount == 0说明切分的都回来了，可以还给PC合并前后页
		if (span->_usecount == 0)
		{
			_CCSpanList[index].Erase(span); // 拿掉
			span->_SpanFreeList = nullptr;
			span->_next = nullptr;
			span->_prev = nullptr;
			// 要排序吗？不用，具体看博客
			_CCSpanList[index].CC_mtx.unlock(); // 解开桶锁，加全锁
			PageCache::GetInstance()->PC_mtx.lock();
			PageCache::GetInstance()->ReleaseSpanToPC(span);
			PageCache::GetInstance()->PC_mtx.unlock();
			_CCSpanList[index].CC_mtx.lock();

		}
		start = next; // 这是一个链式结构，循环把它们还了
	}
	_CCSpanList[index].CC_mtx.unlock();

}