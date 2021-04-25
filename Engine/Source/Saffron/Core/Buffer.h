#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Buffer
{
public:
	Buffer();
	Buffer(uchar* data, uint size);
	template <typename T>
	explicit Buffer(ArrayList<T>& container);
	explicit Buffer(String& string);

	static Buffer Copy(const void* data, uint size);
	static Buffer Copy(const Buffer& buffer);
	template <typename T>
	static Buffer Copy(const ArrayList<T>& container);
	static Buffer Copy(const String& string);
	static Buffer Encapsulate(uchar* data);

	void Allocate(uint size);
	void ZeroInitialize();

	template <typename T>
	T& Read(uint offset = 0);

	void Write(void* data, uint size, uint offset = 0) const;
	void Write(const void* data, uint size, uint offset) const;

	void Destroy();
	void Reset();

	operator bool() const;
	uchar& operator[](int index);
	uchar operator[](int index) const;

	template <typename T>
	T* As();

	uchar* Data();
	const uchar* Data() const;
	uint Size() const;

private:
	uchar* _data;
	uint _size;
};


template <typename T>
Buffer::Buffer(ArrayList<T>& container)
{
	_data = container.data();
	_size = container.size() * sizeof(T);
}

template <typename T>
Buffer Buffer::Copy(const ArrayList<T>& container)
{
	return Copy(container.data(), container.size() * sizeof(T));
}

template <typename T>
T& Buffer::Read(uint offset)
{
	return *reinterpret_cast<T*>(_data + offset);
}

template <typename T>
T* Buffer::As()
{
	return reinterpret_cast<T*>(_data);
}
}
