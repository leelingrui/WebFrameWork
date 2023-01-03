#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <memory>
#include <iostream>
#include <stack>
#include <../thirdpartylib/include/ConcurrentAlloc.h>
#include <atomic>
#define NODE_TAIL 255
#define NODE_HEAD 254
namespace memory
{
	template<typename T, typename _Alloc = concurrent_allocater<T>>
	class MemoryPool
	{
	public:
		MemoryPool()
		{
			m_allocater = nullptr;
			m_deleter = nullptr;
		}
		void free(T* object)
		{
			allocator.deallocate(object, 1);
		}
		T* alloc()
		{
			return reinterpret_cast<T*>(&allocator.allocate(1));
		}
		~MemoryPool() { };
	private:
		T*(*m_allocater)();
		void(*m_deleter)(T*);
		_Alloc allocator;
	};


	template <typename _Tp>
	class concurrent_allocater : std::allocator<_Tp>
	{
	public:
		using value_type = _Tp;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type =  std::size_t;
		using difference_type = std::ptrdiff_t;
		static const size_type _min_M_size = 8;
		pointer address(reference x) const { return &x; };
		const_pointer address(const_reference x) const { return &x; };
		reference allocate(size_type n)
		{
			static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
			pointer old_head = reinterpret_cast<pointer>(ConcurrentAlloc(sizeof(value_type) * n));
			for (int var = 0; var < n; var++)
			{
				construct(old_head + var, _Tp());
			}
			return *old_head;
		}
		void deallocate(pointer p, size_type)
		{
			ConcurrentFree(p);
		}
		size_type max_size() const
		{
			return static_cast<size_type>(-1) / sizeof(value_type);
		}
		void construct(pointer p, const value_type& x)
		{
			new(p) value_type(x);
		}
		void destory(pointer p) { p->~value_type(); };
		concurrent_allocater() {};
		concurrent_allocater(const concurrent_allocater& rval) {};
		concurrent_allocater(const concurrent_allocater&& rval) 
		{
			rval.avaliable = false;
		};
		virtual ~concurrent_allocater()
		{
			avaliable = false;
		};
		concurrent_allocater& operator=(const concurrent_allocater& rval) = delete;
		inline concurrent_allocater& operator=(const concurrent_allocater&& rval)
		{
			*this = rval;
		}
		inline bool constexpr operator==(concurrent_allocater& rval)
		{
			return rval.avaliable == avaliable;
		}
		inline bool constexpr operator!=(concurrent_allocater& rval)
		{
			return rval.avaliable != avaliable;
		}
	protected:
		bool avaliable = true;
	};
}
#endif