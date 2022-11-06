#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <memory>
#include <iostream>
#include <stack>

namespace memory
{
	template<typename _Tp>
	class MemoryPool
	{
	public:
		MemoryPool()
		{
			m_allocater = nullptr;
			m_deleter = nullptr;
		}
		void free(_Tp* object)
		{
			mems.push(object);
		}
		_Tp* alloc()
		{
			if (mems.size())
			{
				_Tp* result = std::move(mems.top());
				mems.pop();
				return result;
			}
			else
			{
				if (m_allocater) return m_allocater();
				else return new _Tp;
			}
		}
		void SetAllocater(_Tp(*allocater)())
		{
			m_allocater = allocater;
		}
		~MemoryPool()
		{
			std::cout << mems.size();
			while (!mems.empty())
			{
				if (m_deleter) m_deleter(mems.top());
				else delete mems.top();
				mems.pop();
			}
		}
	private:
		_Tp*(*m_allocater)();
		void(*m_deleter)(_Tp*);
		std::stack<_Tp*> mems;
	};
}
#endif