#include "SaffronPCH.h"

#include "Saffron/Core/Buffer.h"

namespace Se
{
Buffer::Buffer() :
	Buffer(nullptr, 0)
{
}

Buffer::Buffer(uchar* data, uint size) :
	_data(data),
	_size(size)
{
}

Buffer::Buffer(String& string) :
	_data(reinterpret_cast<uchar*>(string.data())),
	_size(string.length())
{
}

Buffer Buffer::Copy(const void* data, uint size)
{
	Buffer buffer;
	buffer.Allocate(size);
	std::memcpy(buffer._data, data, size);
	return buffer;
}

Buffer Buffer::Copy(const Buffer& buffer)
{
	return Copy(buffer.Data(), buffer.Size());
}

Buffer Buffer::Copy(const String& string)
{
	return Copy(string.data(), string.length() * sizeof(String::value_type));
}

Buffer Buffer::Encapsulate(uchar* data)
{
	Buffer buffer;
	buffer._data = data;
	buffer._size = 0;
	return buffer;
}

void Buffer::Allocate(uint size)
{
	if (_data)
	{
		delete[] _data;
		_data = nullptr;
	}

	if (size == 0) return;

	_data = new uchar[size];
	_size = size;
}

void Buffer::ZeroInitialize()
{
	if (_data) memset(_data, 0, _size);
}

void Buffer::Write(void* data, uint size, uint offset) const
{
	Debug::Assert(offset + size <= _size, "Buffer overflow");;
	memcpy(_data + offset, data, size);
}

void Buffer::Write(const void* data, uint size, uint offset) const
{
	Debug::Assert(offset + size <= _size, "Buffer overflow");;
	memcpy(_data + offset, data, size);
}

void Buffer::Destroy()
{
	if (_data)
	{
		delete[] _data;
	}
	Reset();
}

void Buffer::Reset()
{
	_data = nullptr;
	_size = 0;
}

Buffer::operator bool() const
{
	return _data;
}

uchar& Buffer::operator[](int index)
{
	return _data[index];
}

uchar Buffer::operator[](int index) const
{
	return _data[index];
}

uchar* Buffer::Data()
{
	return _data;
}

const uchar* Buffer::Data() const
{
	return _data;
}

uint Buffer::Size() const
{
	return _size;
}
}
