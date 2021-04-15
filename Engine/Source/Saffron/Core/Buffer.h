#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Buffer
{
public:
	Buffer();
	Buffer(Uint8* data, Uint32 size);
	template <typename T>
	explicit Buffer(ArrayList<T>& container);
	explicit Buffer(String& string);

	static Buffer Copy(const void* data, Uint32 size);
	static Buffer Copy(const Buffer& buffer);
	template <typename T>
	static Buffer Copy(const ArrayList<T>& container);
	static Buffer Copy(const String& string);
	static Buffer Encapsulate(Uint8* data);

	void Allocate(Uint32 size);
	void ZeroInitialize();

	template <typename T>
	T& Read(Uint32 offset = 0);

	void Write(void* data, Uint32 size, Uint32 offset = 0) const;
	void Write(const void* data, Uint32 size, Uint32 offset) const;

	void Destroy();
	void Reset();

	operator bool() const;
	Uint8& operator[](int index);
	Uint8 operator[](int index) const;

	template <typename T>
	T* As();

	Uint8* Data();
	const Uint8* Data() const;
	Uint32 Size() const;

private:
	Uint8* _data;
	Uint32 _size;
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
T& Buffer::Read(Uint32 offset)
{
	return *reinterpret_cast<T*>(_data + offset);
}

template <typename T>
T* Buffer::As()
{
	return reinterpret_cast<T*>(_data);
}
}
