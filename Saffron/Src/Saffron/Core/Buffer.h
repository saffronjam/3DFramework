#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Buffer
{
public:
	Buffer();
	Buffer(Uint8 *data, Uint32 size);

	template<typename T>
	static Buffer Copy(T *data, Uint32 size);
	static Buffer Copy(const Buffer &buffer);
	static Buffer Encapsulate(Uint8 *data);

	void Allocate(Uint32 size);
	void ZeroInitialize();

	template<typename T>
	T &Read(Uint32 offset = 0);

	void Write(void *data, Uint32 size, Uint32 offset = 0) const;
	void Write(const void *data, Uint32 size, Uint32 offset) const;

	void Destroy();
	void Reset();

	operator bool() const;
	Uint8 &operator[](int index);
	Uint8 operator[](int index) const;

	template<typename T>
	T *As();


	Uint8 *Data() { return m_Data; }
	const Uint8 *Data() const { return m_Data; }
	Uint32 Size() const { return m_Size; }


private:
	Uint8 *m_Data;
	Uint32 m_Size;
};


template <typename T>
Buffer Buffer::Copy(T *data, Uint32 size)
{
	Buffer buffer;
	buffer.Allocate(size);
	memcpy(buffer.m_Data, data, size);
	return buffer;
}

template <typename T>
T &Buffer::Read(Uint32 offset)
{
	return *reinterpret_cast<T *>(m_Data + offset);
}

template <typename T>
T *Buffer::As()
{
	return reinterpret_cast<T *>(m_Data);
}

}

