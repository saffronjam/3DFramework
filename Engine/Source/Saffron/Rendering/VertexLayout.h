#pragma once

#include <d3d11.h>

#include "Saffron/Base.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
enum class ElementType
{
	Position2D,
	Position3D,
	Count
};

class VertexElement
{
public:
	VertexElement(ElementType type);

	auto Type() const -> ElementType;

	auto Name() const -> const char*;
	auto Format() const -> DXGI_FORMAT;
	auto ByteSize() const -> uint;
	auto Code() const -> const char*;

	template <template<ElementType> class Executor, typename ... Args>
	static constexpr auto Bridge(ElementType type, Args&& ... args)
	{
		switch (type)
		{
		case ElementType::Position2D:
		{
			return Executor<ElementType::Position2D>::Exec(std::forward<Args>(args)...);
		}
		case ElementType::Position3D:
		{
			return Executor<ElementType::Position3D>::Exec(std::forward<Args>(args)...);
		}
		default:
		{
			Debug::Break("Invalid {} when bridging", typeid(ElementType).name());
			return Executor<ElementType::Count>::Exec(std::forward<Args>(args)...);
		}
		}
	}

private:
	ElementType _type;
};

template <ElementType Type>
struct InputLayoutElementMap
{
};

template <>
struct InputLayoutElementMap<ElementType::Position2D>
{
	static constexpr const char* Name = "Position2D";
	static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr size_t ByteSize = 8ull;
	static constexpr const char* Code = "P2";
};

template <>
struct InputLayoutElementMap<ElementType::Position3D>
{
	static constexpr const char* Name = "Position3D";
	static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr size_t ByteSize = 12ull;
	static constexpr const char* Code = "P3";
};

template <>
struct InputLayoutElementMap<ElementType::Count>
{
	static constexpr const char* Name = "Invalid";
	static constexpr DXGI_FORMAT Format = static_cast<DXGI_FORMAT>(0);
	static constexpr size_t ByteSize = 0ull;
	static constexpr const char* Code = "??";
};

namespace Bridge
{
template <ElementType Type, typename ... Args>
struct VertexElementNameLookUp
{
	static constexpr auto Exec()
	{
		return InputLayoutElementMap<Type>::Name;
	}
};

template <ElementType Type, typename ... Args>
struct VertexElementFormatLookUp
{
	static constexpr auto Exec()
	{
		return InputLayoutElementMap<Type>::Format;
	}
};

template <ElementType Type, typename ... Args>
struct VertexElementByteSizeLookUp
{
	static constexpr auto Exec()
	{
		return InputLayoutElementMap<Type>::ByteSize;
	}
};

template <ElementType Type, typename ... Args>
struct VertexElementCodeLookUp
{
	static constexpr auto Exec()
	{
		return InputLayoutElementMap<Type>::Code;
	}
};
}


class VertexLayout
{
public:
	VertexLayout(std::initializer_list<VertexElement> elements);

	auto Descs() const -> const std::vector<D3D11_INPUT_ELEMENT_DESC>&;
	auto Code() const -> const std::string&;
	auto ByteSize() const -> size_t;

private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> _descs{};
	std::vector<VertexElement> _elements;

	std::string _code;
	size_t _byteSize;
};
}
