#include "SaffronPCH.h"

#include <numeric>

#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
VertexElement::VertexElement(std::string name, ElementType type) :
	_name(std::move(name)),
	_type(type)
{
}

auto VertexElement::Type() const -> ElementType
{
	return _type;
}

auto VertexElement::Name() const -> const std::string&
{
	return _name;
}

auto VertexElement::Format() const -> DXGI_FORMAT
{
	return Bridge<Bridge::VertexElementFormatLookUp>(_type);
}

auto VertexElement::ByteSize() const -> uint
{
	return Bridge<Bridge::VertexElementByteSizeLookUp>(_type);
}

auto VertexElement::Code() const -> const char*
{
	return Bridge<Bridge::VertexElementCodeLookUp>(_type);
}

VertexLayout::VertexLayout(std::initializer_list<VertexElement> elements) :
	_elements(elements),
	_code(
		std::accumulate(
			_elements.begin(),
			_elements.end(),
			std::string{},
			[](std::string dest, const VertexElement& element)
			{
				return std::move(dest) + element.Code();
			}
		)
	),
	_byteSize(
		std::accumulate(
			_elements.begin(),
			_elements.end(),
			0ull,
			[](size_t dest, const VertexElement& element)
			{
				return dest + element.ByteSize();
			}
		)
	)
{
	_descs.reserve(_elements.size());

	uint offset = 0;
	for (const auto& element : _elements)
	{
		_descs.emplace_back(element.Name().c_str(), 0, element.Format(), 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0);
		offset += element.ByteSize();
	}
}

VertexLayout::VertexLayout(const VertexLayout& other) :
	_elements(other._elements),
	_descs(other._descs),
	_byteSize(other._byteSize),
	_code(other._code)
{
	for (int i = 0; i < _descs.size(); i ++)
	{
		_descs[i].SemanticName = _elements[i].Name().c_str();
	}
}

auto VertexLayout::operator=(const VertexLayout& other) -> VertexLayout&
{
	_elements = other._elements;
	_descs = other._descs;
	_byteSize = other._byteSize;
	_code = other._code;

	for (int i = 0; i < _descs.size(); i++)
	{
		_descs[i].SemanticName = _elements[i].Name().c_str();
	}

	return *this;
}

auto VertexLayout::Descs() const -> const std::vector<D3D11_INPUT_ELEMENT_DESC>&
{
	return _descs;
}

auto VertexLayout::Code() const -> const std::string&
{
	return _code;
}

auto VertexLayout::ByteSize() const -> size_t
{
	return _byteSize;
}
}
