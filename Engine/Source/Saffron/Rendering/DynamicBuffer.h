#pragma once

#include "Saffron/Base.h"

namespace Se::Dcb
{
#define LEAF_ELEMENT_TYPES \
	X( Type::Float ) \
	X( Type::Float2 ) \
	X( Type::Float3 ) \
	X( Type::Float4 ) \
	X( Type::Matrix ) \
	X( Type::Bool ) \
	X( Type::Integer )


enum class Type
{
	// Leaf types
	Float,
	Float2,
	Float3,
	Float4,
	Matrix,
	Bool,
	Integer,
	// Non-leaf types
	Struct,
	Array,
	Empty,
	// -
	Count
};

template <Type type>
struct Map
{
	static constexpr bool Valid = false;
};

template <typename T>
struct ReverseMap
{
	static constexpr bool Valid = false;
};

struct Functors
{
	template <Type type>
	struct SameSysType
	{
		static constexpr auto Exec(const std::type_info& typeInfo)
		{
			return typeInfo == typeid(Map<type>::SysType);
		}
	};

	template <Type type>
	struct ExtractHLSLSize
	{
		static constexpr auto Exec()
		{
			return Map<type>::HlslSize;
		}
	};

	template <Type type>
	struct ExtractCode
	{
		static constexpr auto Exec()
		{
			return Map<type>::Code;
		}
	};
};


class LayoutElement
{
	struct DataExtensionBase
	{
		virtual ~DataExtensionBase() = default;
	};

	friend class ConstElementRef;
	friend class ElementRef;
	friend class EditableLayout;
	friend struct DataExtension;

public:
	LayoutElement& operator[](const String& key);
	const LayoutElement& operator[](const String& key) const;

	LayoutElement& ArrayType();
	const LayoutElement& ArrayType() const;

	bool Exists() const;

	Type GetType() const;
	String GetSignature() const;
	size_t GetOffset() const;

	template <class T>
	size_t GetOffset() const
	{
		SE_CORE_ASSERT(Bridge<Functors::SameSysType>(_type, typeid(T)), "Tried to resolve non-leaf element");
		return GetOffset();
	}

	size_t GetOffsetAfter() const;
	size_t GetSize() const;

	LayoutElement& Add(Type add, String name);

	template <Type add>
	LayoutElement& Add(String name)
	{
		return Add(add, name);
	}

	LayoutElement& Set(Type add, size_t size);

	template <Type add>
	LayoutElement& Set(size_t size)
	{
		return Set(add, size);
	}

	static LayoutElement& Empty();

private:
	LayoutElement() = default;
	explicit LayoutElement(Type type);

	size_t Finalize(size_t offset);
	size_t FinalizeStruct(size_t offset);
	size_t FinalizeArray(size_t offset);

	Pair<size_t, const LayoutElement*> CalculateIndexOffset(size_t offset, size_t index) const;

	String GetStructSignature() const;
	String GetArraySignature() const;

	static bool ValidateName(const String& name);
	static bool IsLeafType(Type type);

	static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size);
	static size_t AdvanceToBoundary(size_t offset);
	static bool CrossesBoundary(size_t offset, size_t size);

private:
	Type _type = Type::Empty;
	size_t _offset = 0;
	Unique<DataExtensionBase> _dataExtension{};

public:
	template <template<Type> class F, typename... Args>
	static constexpr auto Bridge(Type type, Args&&... args)
	{
#define X(el) case el: \
		return F<el>::Exec(std::forward<Args>(args)...);

		SE_CORE_ASSERT(IsLeafType(type) && "InValid element type");
		switch (type)
		{
		LEAF_ELEMENT_TYPES;
		default: return F<Type::Integer>::Exec(std::forward<Args>(args)...);
		}
#undef X
	}
};

class Layout
{
	friend class LayoutCodex;
	friend class DynBuffer;

public:
	size_t GetSize() const;
	String GetSignature() const;

protected:
	explicit Layout(Shared<LayoutElement> root);

protected:
	Shared<LayoutElement> _root;
};

class EditableLayout : public Layout
{
	friend class LayoutCodex;

public:
	EditableLayout();

	LayoutElement& operator[](const String& key);
	const LayoutElement& operator[](const String& key) const;

	template <Type type>
	LayoutElement& Add(const String& key)
	{
		return _root->Add<type>(key);
	}

private:
	void ClearRoot();
	Shared<LayoutElement> DeliverRoot();
};

class ReadonlyLayout
{
	friend class LayoutCodex;
	friend class Buffer;

public:
	const LayoutElement& operator[](const String& key) const;

	const Shared<LayoutElement>& ShareRoot() const;
	Shared<LayoutElement> PilferRoot() const;

private:
	explicit ReadonlyLayout(Shared<LayoutElement> root);

private:
	Shared<LayoutElement> _root;
};


class ConstElementRef
{
	friend class Buffer;
	friend class ElementRef;

public:
	class Ptr
	{
		friend ConstElementRef;

	public:
		template <typename T>
		operator const T*() const
		{
			SE_CORE_STATIC_ASSERT(ReverseMap<std::remove_const<T>>::Valid,
			                      "Unsupported SysType used in pointer conversion");
			return &static_cast<const T&>(*_ref);
		}

	private:
		explicit Ptr(const ConstElementRef* ref);

	private:
		const ConstElementRef* _ref;
	};

public:
	Ptr operator&() const;
	ConstElementRef operator[](const String& key) const;
	ConstElementRef operator[](size_t index) const;

	bool Exists() const;

	template <typename T>
	operator const T&() const
	{
		SE_CORE_STATIC_ASSERT(ReverseMap<std::remove_const<T>>::Valid, "Unsupported SysType used in conversion");
		return *reinterpret_cast<const T*>(_data + _offset + _layoutElement->GetOffset<T>());
	}

private:
	ConstElementRef(const LayoutElement* layoutElement, const char* data, size_t offset);

private:
	size_t _offset;
	const char* _data;
	const LayoutElement* _layoutElement;
};

class ElementRef
{
	friend class Buffer;

public:
	class Ptr
	{
		friend ElementRef;

	public:
		template <typename T>
		operator T*() const
		{
			SE_CORE_STATIC_ASSERT(ReverseMap<std::remove_const<T>>::Valid,
			                      "Unsupported SysType used in pointer conversion");
			return &static_cast<T&>(*_ref);
		}

	private:
		explicit Ptr(ElementRef* ref);

	private:
		ElementRef* _ref;
	};

public:
	operator ConstElementRef() const;
	Ptr operator&() const;
	ElementRef operator[](const String& key) const;
	ElementRef operator[](size_t index) const;

	bool Exists() const { return _layoutElement->Exists(); }

	template <typename T>
	bool SetIfExists(const T& value)
	{
		if (Exists())
		{
			*this = value;
			return true;
		}
		return false;
	}

	template <typename T>
	explicit operator T&() const
	{
		SE_CORE_STATIC_ASSERT(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in conversion");
		return *reinterpret_cast<T*>(_data + _offset + _layoutElement->GetOffset<T>());
	}

	template <typename T>
	T& operator=(const T& rhs)
	{
		SE_CORE_STATIC_ASSERT(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in assignment");
		return static_cast<T&>(*this) = rhs;
	}

private:
	ElementRef(const LayoutElement* layoutElement, char* data, size_t offset);

private:
	size_t _offset;
	char* _data;
	const LayoutElement* _layoutElement;
};

class LayoutCodex
{
public:
	static ReadonlyLayout Resolve(EditableLayout&& layout);

private:
	static LayoutCodex& Instance() noexcept;
	UnorderedMap<String, Shared<LayoutElement>> _map{};
};

class Buffer
{
public:
	explicit Buffer(EditableLayout&& layout);
	explicit Buffer(const ReadonlyLayout& layout);
	explicit Buffer(ReadonlyLayout&& layout) noexcept;
	explicit Buffer(const Buffer& buffer);
	explicit Buffer(Buffer&& buffer) noexcept;
	Buffer& operator=(const Buffer &buffer);

	ElementRef operator[](const String& key);
	ConstElementRef operator[](const String& key) const;

	const char* GetData() const;
	size_t GetSize() const;
	const LayoutElement& GetRoot() const;

	Shared<LayoutElement> ShareLayoutRoot() const;

private:
	Shared<LayoutElement> _root{};
	ArrayList<char> _data{};
};


template <>
struct Map<Type::Float>
{
	using SysType = float; // type used in the CPU side
	static constexpr size_t HlslSize = sizeof(SysType); // size of type on GPU side
	static constexpr const char* Code = "F1"; // Code used for generating signature of layout
	static constexpr bool Valid = true; // metaprogramming flag to check Validity of Map <param>
};

template <>
struct Map<Type::Float2>
{
	using SysType = Vector2;
	static constexpr size_t HlslSize = sizeof(SysType);
	static constexpr const char* Code = "F2";
	static constexpr bool Valid = true;
};

template <>
struct Map<Type::Float3>
{
	using SysType = Vector3;
	static constexpr size_t HlslSize = sizeof(SysType);
	static constexpr const char* Code = "F3";
	static constexpr bool Valid = true;
};

template <>
struct Map<Type::Float4>
{
	using SysType = Vector4;
	static constexpr size_t HlslSize = sizeof(SysType);
	static constexpr const char* Code = "F4";
	static constexpr bool Valid = true;
};

template <>
struct Map<Type::Matrix>
{
	using SysType = Matrix;
	static constexpr size_t HlslSize = sizeof(SysType);
	static constexpr const char* Code = "M4";
	static constexpr bool Valid = true;
};

template <>
struct Map<Type::Bool>
{
	using SysType = bool;
	static constexpr size_t HlslSize = 4u;
	static constexpr const char* Code = "BL";
	static constexpr bool Valid = true;
};

template <>
struct Map<Type::Integer>
{
	using SysType = int;
	static constexpr size_t HlslSize = sizeof(SysType);
	static constexpr const char* Code = "IN";
	static constexpr bool Valid = true;
};

#define X(el) static_assert(Map<el>::Valid,"Missing map implementation for " #el);
LEAF_ELEMENT_TYPES;
#undef X

#define X(el) \
	template<> struct ReverseMap<Map<el>::SysType> \
	{ \
		static constexpr Type Type = el; \
		static constexpr bool Valid = true; \
	};

LEAF_ELEMENT_TYPES;
#undef X
}
