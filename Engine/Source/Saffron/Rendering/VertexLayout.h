#pragma once

#include "Saffron/Base.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
enum class ElementType
{
	Float2,
	Float3,
	Float4,
	Count
};

class VertexElement
{
public:
	VertexElement(std::string name, ElementType type);

	auto Type() const -> ElementType;

	auto Name() const -> const std::string&;
	auto Format() const -> int;
	auto ByteSize() const -> uint;
	auto Code() const -> const char*;

	template <template<ElementType> class Executor, typename ... Args>
	static constexpr auto Bridge(ElementType type, Args&& ... args)
	{
		switch (type)
		{
		case ElementType::Float2:
		{
			return Executor<ElementType::Float2>::Exec(std::forward<Args>(args)...);
		}
		case ElementType::Float3:
		{
			return Executor<ElementType::Float3>::Exec(std::forward<Args>(args)...);
		}
		case ElementType::Float4:
		{
			return Executor<ElementType::Float4>::Exec(std::forward<Args>(args)...);
		}
		default:
		{
			Debug::Break("Invalid {} when bridging", typeid(ElementType).name());
			return Executor<ElementType::Count>::Exec(std::forward<Args>(args)...);
		}
		}
	}

private:
	std::string _name;
	ElementType _type;
};

template <ElementType Type>
struct InputLayoutElementMap
{
};

template <>
struct InputLayoutElementMap<ElementType::Float2>
{
	static constexpr int Format = 1;
	static constexpr size_t ByteSize = 8ull;
	static constexpr const char* Code = "F2";
};

template <>
struct InputLayoutElementMap<ElementType::Float3>
{
	static constexpr int Format = 2;
	static constexpr size_t ByteSize = 12ull;
	static constexpr const char* Code = "F3";
};

template <>
struct InputLayoutElementMap<ElementType::Float4>
{
	static constexpr int Format = 3;
	static constexpr size_t ByteSize = 16ull;
	static constexpr const char* Code = "F4";
};

template <>
struct InputLayoutElementMap<ElementType::Count>
{
	static constexpr int Format = 0;
	static constexpr size_t ByteSize = 0ull;
	static constexpr const char* Code = "??";
};

namespace Bridge
{
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
	VertexLayout(const VertexLayout& other);
	auto operator =(const VertexLayout& other) -> VertexLayout&;

	auto Descs() const -> const std::vector<int>&;
	auto Code() const -> const std::string&;
	auto ByteSize() const -> size_t;

private:
	std::vector<int> _descs{};
	std::vector<VertexElement> _elements;

	std::string _code;
	size_t _byteSize;
};
}
