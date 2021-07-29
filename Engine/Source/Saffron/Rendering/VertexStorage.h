#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
class VertexStorage
{
public:
	explicit VertexStorage(VertexLayout layout);

	template <typename VertexType>
	void Add(VertexType type);

	auto Data() -> uchar*;
	auto Data() const -> const uchar*;

	auto Count() const -> size_t;
	auto ByteSize() const -> size_t;
	auto Stride() const -> size_t;

private:
	VertexLayout _layout;
	std::vector<uchar> _data;
};

template <typename VertexType>
void VertexStorage::Add(VertexType type)
{
	constexpr auto VertexByteSize = sizeof VertexType;

	Debug::Assert(
		VertexByteSize == _layout.ByteSize(),
		"VertexType size did not match size in VertexLayout specification"
	);

	const size_t before = _data.size();
	_data.resize(before + VertexByteSize);
	
	std::memcpy(&_data[before], reinterpret_cast<uchar*>(&type), VertexByteSize);
}
}
