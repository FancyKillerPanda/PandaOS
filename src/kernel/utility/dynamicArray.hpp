//  ===== Date Created: 10 August, 2021 ===== 

#if !defined(DYNAMICARRAY_HPP)
#define DYNAMICARRAY_HPP

#include "memory/heapAllocator.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

template <typename T>
struct DynamicArray
{
	T* data = nullptr;
	usize size = 0;
	usize capacity = 0;

	void push(T&& element)
	{
		if (size >= capacity)
		{
			expand();
		}

		ASSERT(size < capacity, "Size must be less than capacity!");
		
		data[size] = element;
		size += 1;
	}

	T&& pop()
	{
		size -= 1;
		return (T&&) data[size];
	}

	T& operator[](usize index)
	{
		ASSERT(index < size, "Dynamic array access out of bounds.");
		return data[index];
	}

	void expand(usize newCapacity = 0)
	{
		if (newCapacity == 0)
		{
			if (capacity == 0)
			{
				newCapacity = 1;
			}
			else
			{
				newCapacity = capacity * 2;
			}
		}

		ASSERT(newCapacity > capacity, "New capacity must be greater than the current capacity.");
		capacity = newCapacity;

		T* temp = (T*) malloc(newCapacity * sizeof(T));
		memcpy(temp, data, size * sizeof(T));
		free(data);
		data = temp;
	}
};

#endif
