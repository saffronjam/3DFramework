#include "SaffronPCH.h"

#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Assert.h"

namespace Se
{
Buffer::Buffer() : Buffer(nullptr, 0) {}

Buffer::Buffer(Uint8 *data, Uint32 size) : m_Data(data), m_Size(size) {}

Buffer Buffer::Copy(const void *data, Uint32 size)
{
	Buffer buffer;
	buffer.Allocate(size);
	memcpy(buffer.m_Data, data, size);
	return buffer;
}

Buffer Buffer::Copy(const Buffer &buffer)
{
	return Copy(buffer.Data(), buffer.Size());
}

Buffer Buffer::Encapsulate(Uint8 *data)
{
	Buffer buffer;
	buffer.m_Data = data;
	buffer.m_Size = 0;
	return buffer;
}

void Buffer::Allocate(Uint32 size)
{
	if ( m_Data )
	{
		delete[] m_Data;
		m_Data = nullptr;
	}

	if ( size == 0 )
		return;

	m_Data = new Uint8[size];
	m_Size = size;
}

void Buffer::ZeroInitialize()
{
	if ( m_Data )
		memset(m_Data, 0, m_Size);
}

void Buffer::Write(void *data, Uint32 size, Uint32 offset) const
{
	SE_CORE_ASSERT(offset + size <= m_Size, "Buffer overflow");
	memcpy(m_Data + offset, data, size);
}

void Buffer::Write(const void *data, Uint32 size, Uint32 offset) const
{
	SE_CORE_ASSERT(offset + size <= m_Size, "Buffer overflow");
	memcpy(m_Data + offset, data, size);
}

void Buffer::Destroy()
{
	if ( m_Data )
	{
		delete[] m_Data;
	}
	Reset();
}

void Buffer::Reset()
{
	m_Data = nullptr;
	m_Size = 0;
}

Buffer::operator bool() const
{
	return m_Data;
}

Uint8 &Buffer::operator[](int index)
{
	return m_Data[index];
}

Uint8 Buffer::operator[](int index) const
{
	return m_Data[index];
}
}
