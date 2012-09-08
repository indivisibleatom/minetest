/*
Minetest-c55
Copyright (C) 2010-2012 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef UTIL_POINTER_HEADER
#define UTIL_POINTER_HEADER

#include "../irrlichttypes.h"
#include "../debug.h" // For assert()
#include <cstring>

template <typename T>
class SharedPtr
{
public:
	SharedPtr(T *t=NULL)
	{
		refcount = new int;
		*refcount = 1;
		ptr = t;
	}
	SharedPtr(const SharedPtr<T> &t)
	{
		refcount = t.refcount;
		(*refcount)++;
		ptr = t.ptr;
	}
	~SharedPtr()
	{
		drop();
	}
	SharedPtr<T> & operator=(T *t)
	{
		drop();
		refcount = new int;
		*refcount = 1;
		ptr = t;
		return *this;
	}
	SharedPtr<T> & operator=(const SharedPtr<T> &t)
	{
		drop();
		refcount = t.refcount;
		(*refcount)++;
		ptr = t.ptr;
		return *this;
	}
	T* get() const
	{
		return ptr;
	}
	T* operator->() const
	{
		return ptr;
	}
	T & operator*() const
	{
		return *ptr;
	}
	bool operator!=(T *t) const
	{
		return ptr != t;
	}
	bool operator==(T *t) const
	{
		return ptr == t;
	}
	T & operator[](unsigned int i) const
	{
		return ptr[i];
	}
private:
	void drop()
	{
		assert((*refcount) > 0);
		(*refcount)--;
		if(*refcount == 0)
		{
			delete refcount;
			if(ptr != NULL)
				delete ptr;
		}
	}
	T *ptr;
	int *refcount;
};

template <typename T>
class HybridPtr
{
public:
	HybridPtr(T *t=NULL)
	{
		refcount = new int;
		*refcount = 1;
		ptr = t;
		never_delete = false;
	}
	HybridPtr(const HybridPtr<T> &t)
	{
		refcount = t.refcount;
		(*refcount)++;
		ptr = t.ptr;
		never_delete = t.never_delete;
	}
	~HybridPtr()
	{
		drop();
	}
	HybridPtr<T> & operator=(T *t)
	{
		drop();
		refcount = new int;
		*refcount = 1;
		ptr = t;
		never_delete = false;
		return *this;
	}
	HybridPtr<T> & operator=(const HybridPtr<T> &t)
	{
		drop();
		refcount = t.refcount;
		(*refcount)++;
		ptr = t.ptr;
		never_delete = t.never_delete;
		return *this;
	}
	T* get() const
	{
		return ptr;
	}
	T* operator->() const
	{
		return ptr;
	}
	T & operator*() const
	{
		return *ptr;
	}
	bool operator!=(T *t) const
	{
		return ptr != t;
	}
	bool operator==(T *t) const
	{
		return ptr == t;
	}
	T & operator[](unsigned int i) const
	{
		return ptr[i];
	}
	// Can be used for transparently passing references to global things
	void setNeverDelete(bool a_never_delete)
	{
		never_delete = a_never_delete;
	}
	bool getNeverDelete() const
	{
		return never_delete;
	}
private:
	void drop()
	{
		assert((*refcount) > 0);
		(*refcount)--;
		if(*refcount == 0)
		{
			delete refcount;
			if(ptr != NULL && !never_delete)
				delete ptr;
		}
	}
	T *ptr;
	int *refcount;
	bool never_delete;
};

template <typename T>
class Buffer
{
public:
	Buffer()
	{
		m_size = 0;
		data = NULL;
	}
	Buffer(unsigned int size)
	{
		m_size = size;
		if(size != 0)
			data = new T[size];
		else
			data = NULL;
	}
	Buffer(const Buffer &buffer)
	{
		m_size = buffer.m_size;
		if(m_size != 0)
		{
			data = new T[buffer.m_size];
			memcpy(data, buffer.data, buffer.m_size);
		}
		else
			data = NULL;
	}
	Buffer(const T *t, unsigned int size)
	{
		m_size = size;
		if(size != 0)
		{
			data = new T[size];
			memcpy(data, t, size);
		}
		else
			data = NULL;
	}
	~Buffer()
	{
		drop();
	}
	Buffer& operator=(const Buffer &buffer)
	{
		if(this == &buffer)
			return *this;
		drop();
		m_size = buffer.m_size;
		if(m_size != 0)
		{
			data = new T[buffer.m_size];
			memcpy(data, buffer.data, buffer.m_size);
		}
		else
			data = NULL;
		return *this;
	}
	T & operator[](unsigned int i) const
	{
		return data[i];
	}
	T * operator*() const
	{
		return data;
	}
	unsigned int getSize() const
	{
		return m_size;
	}
private:
	void drop()
	{
		if(data)
			delete[] data;
	}
	T *data;
	unsigned int m_size;
};

template <typename T>
class SharedBuffer
{
public:
	SharedBuffer()
	{
		m_size = 0;
		data = NULL;
		refcount = new unsigned int;
		(*refcount) = 1;
	}
	SharedBuffer(unsigned int size)
	{
		m_size = size;
		if(m_size != 0)
			data = new T[m_size];
		else
			data = NULL;
		refcount = new unsigned int;
		(*refcount) = 1;
	}
	SharedBuffer(const SharedBuffer &buffer)
	{
		//std::cout<<"SharedBuffer(const SharedBuffer &buffer)"<<std::endl;
		m_size = buffer.m_size;
		data = buffer.data;
		refcount = buffer.refcount;
		(*refcount)++;
	}
	SharedBuffer & operator=(const SharedBuffer & buffer)
	{
		//std::cout<<"SharedBuffer & operator=(const SharedBuffer & buffer)"<<std::endl;
		if(this == &buffer)
			return *this;
		drop();
		m_size = buffer.m_size;
		data = buffer.data;
		refcount = buffer.refcount;
		(*refcount)++;
		return *this;
	}
	/*
		Copies whole buffer
	*/
	SharedBuffer(T *t, unsigned int size)
	{
		m_size = size;
		if(m_size != 0)
		{
			data = new T[m_size];
			memcpy(data, t, m_size);
		}
		else
			data = NULL;
		refcount = new unsigned int;
		(*refcount) = 1;
	}
	/*
		Copies whole buffer
	*/
	SharedBuffer(const Buffer<T> &buffer)
	{
		m_size = buffer.getSize();
		if(m_size != 0)
		{
			data = new T[m_size];
			memcpy(data, *buffer, buffer.getSize());
		}
		else
			data = NULL;
		refcount = new unsigned int;
		(*refcount) = 1;
	}
	~SharedBuffer()
	{
		drop();
	}
	T & operator[](unsigned int i) const
	{
		//assert(i < m_size)
		return data[i];
	}
	T * operator*() const
	{
		return data;
	}
	unsigned int getSize() const
	{
		return m_size;
	}
	operator Buffer<T>() const
	{
		return Buffer<T>(data, m_size);
	}
private:
	void drop()
	{
		assert((*refcount) > 0);
		(*refcount)--;
		if(*refcount == 0)
		{
			if(data)
				delete[] data;
			delete refcount;
		}
	}
	T *data;
	unsigned int m_size;
	unsigned int *refcount;
};

inline SharedBuffer<u8> SharedBufferFromString(const char *string)
{
	SharedBuffer<u8> b((u8*)string, strlen(string)+1);
	return b;
}

#endif

