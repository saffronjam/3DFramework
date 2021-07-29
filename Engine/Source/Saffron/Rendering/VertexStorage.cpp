#include "SaffronPCH.h"

#include "Saffron/Rendering/VertexStorage.h"

namespace Se
{
VertexStorage::VertexStorage(VertexLayout layout) :
	_layout(std::move(layout)),
	_data()
{
}

auto VertexStorage::Data() -> uchar*
{
	return _data.data();
}

auto VertexStorage::Data() const -> const uchar*
{
	return _data.data();
}

auto VertexStorage::Count() const -> size_t
{
	return _data.size() / _layout.ByteSize();
}

auto VertexStorage::ByteSize() const -> size_t
{
	return _data.size();
}

auto VertexStorage::Stride() const -> size_t
{
	return _layout.ByteSize();
}
}
