#include "Saffron/SaffronPCH.h"
#include "Buffer.h"

namespace Se
{
Buffer::Buffer()
	: Buffer(nullptr, 0)
{
}

Buffer::Buffer(Uint8 *data, Uint32 size)
	: m_Data(new Uint8[size]),
	m_Size(size)
{
}

Buffer Buffer::Copy(const Buffer &buffer)
{
	return Copy(buffer.Data(), buffer.Size());
}

void Buffer::Allocate(Uint32 size)
{
	delete[] m_Data;
	m_Data = nullptr;

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
