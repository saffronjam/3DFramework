#include "SaffronPCH.h"

#include "Saffron/Rendering/DynamicBuffer.h"

namespace Se::Dcb
{
struct DataExtension
{
	struct Struct : LayoutElement::DataExtensionBase
	{
		ArrayList<Pair<String, LayoutElement>> Elements{};
	};

	struct Array : LayoutElement::DataExtensionBase
	{
		LayoutElement Element;
		size_t ElementSize;
		size_t Size;
	};
};

LayoutElement& LayoutElement::operator[](const String& key)
{
	SE_CORE_ASSERT(_type == Type::Struct, "Trying to get field from non-struct element");
	auto& elements = static_cast<DataExtension::Struct&>(*_dataExtension).Elements;

	auto result = std::find_if(elements.begin(), elements.end(), [&key](const auto& pair)
	{
		return pair.first == key;
	});
	if (result == elements.end())
	{
		return Empty();
	}
	return result->second;
}

const LayoutElement& LayoutElement::operator[](const String& key) const
{
	return const_cast<LayoutElement&>(*this)[key];
}

LayoutElement& LayoutElement::ArrayType()
{
	SE_CORE_ASSERT(_type == Type::Struct, "Trying to get array-type from non-array element");
	return static_cast<DataExtension::Array&>(*_dataExtension).Element;
}

const LayoutElement& LayoutElement::ArrayType() const
{
	return const_cast<LayoutElement&>(*this).ArrayType();
}

bool LayoutElement::Exists() const
{
	return _type != Type::Empty;
}

Type LayoutElement::GetType() const
{
	return _type;
}

String LayoutElement::GetSignature() const
{
	if (IsLeafType(_type))
	{
		return Bridge<Functors::ExtractCode>(_type);
	}

	switch (_type)
	{
	case Type::Struct:
	{
		return GetStructSignature();
	}
	case Type::Array:
	{
		return GetArraySignature();
	}
	default: SE_CORE_FALSE_ASSERT("Tried to get signature of empty or invalid element");
		return "INVALID";
	}
}

size_t LayoutElement::GetOffset() const
{
	return _offset;
}

size_t LayoutElement::GetOffsetAfter() const
{
	if (IsLeafType(_type))
	{
		return GetOffset() + Bridge<Functors::ExtractHLSLSize>(_type);
	}

	switch (_type)
	{
	case Type::Struct:
	{
		const auto& data = static_cast<DataExtension::Struct&>(*_dataExtension);
		return AdvanceToBoundary(data.Elements.rbegin()->second.GetOffsetAfter());
	}
	case Type::Array:
	{
		const auto& data = static_cast<DataExtension::Array&>(*_dataExtension);
		return _offset + AdvanceToBoundary(data.Element.GetSize()) * data.Size;
	}
	default: SE_CORE_FALSE_ASSERT("Tried to get offset of empty or invalid element");
		return 0u;
	}
}

size_t LayoutElement::GetSize() const
{
	return GetOffsetAfter() - GetOffset();
}

LayoutElement& LayoutElement::Add(Type add, String name)
{
	SE_CORE_ASSERT(_type == Type::Struct, "Can only Add on Struct");
	SE_CORE_ASSERT(ValidateName(name), "Invalid symbol name in Struct");

	auto& structData = static_cast<DataExtension::Struct&>(*_dataExtension);
	auto& list = structData.Elements;

	SE_CORE_ASSERT(!std::any_of(list.begin(), list.end(), [&name](const auto &pair) { return pair.first == name; }),
	               "Duplicate names in Struct");

	list.emplace_back(Move(name), LayoutElement{add});
	return *this;
}

LayoutElement& LayoutElement::Set(Type add, size_t size)
{
	SE_CORE_ASSERT(_type == Type::Array, "Can only Set on Array");
	SE_CORE_ASSERT(size != 0, "Invalid size");

	auto& arrayData = static_cast<DataExtension::Array&>(*_dataExtension);
	arrayData.Element = LayoutElement{add};
	arrayData.Size = size;
	return *this;
}

LayoutElement& LayoutElement::Empty()
{
	static LayoutElement empty{};
	return empty;
}

LayoutElement::LayoutElement(Type type) :
	_type(type)
{
	SE_CORE_ASSERT(_type != Type::Empty, "Tried to manually contruct an element of type Empty");
	if (_type == Type::Struct)
	{
		_dataExtension = CreateUnique<DataExtension::Struct>();
	}
	else if (_type == Type::Array)
	{
		_dataExtension = CreateUnique<DataExtension::Array>();
	}
}

size_t LayoutElement::Finalize(size_t offset)
{
	if (IsLeafType(_type))
	{
		const auto hlslSize = Bridge<Functors::ExtractHLSLSize>(_type);
		_offset = AdvanceIfCrossesBoundary(offset, hlslSize);
		return _offset + hlslSize;
	}

	switch (_type)
	{
	case Type::Struct:
	{
		return FinalizeStruct(offset);
	}
	case Type::Array:
	{
		return FinalizeArray(offset);
	}
	default:
	{
		SE_CORE_FALSE_ASSERT("Tried to finalize empty or invalid element");
		return 0u;
	}
	}
}

size_t LayoutElement::FinalizeStruct(size_t offset)
{
	auto& data = static_cast<DataExtension::Struct&>(*_dataExtension);
	SE_CORE_ASSERT(data.Elements.size() != 0u);
	_offset = AdvanceToBoundary(offset);
	auto offsetNext = offset;
	for (auto& [key, element] : data.Elements)
	{
		offsetNext = element.Finalize(offsetNext);
	}
	return offsetNext;
}

size_t LayoutElement::FinalizeArray(size_t offset)
{
	auto& data = static_cast<DataExtension::Array&>(*_dataExtension);
	SE_CORE_ASSERT(data.Size != 0u);
	_offset = AdvanceToBoundary(offset);
	data.Element.Finalize(offset);
	data.ElementSize = AdvanceToBoundary(data.Element.GetSize());
	return GetOffsetAfter();
}

Pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexOffset(size_t offset, size_t index) const
{
	SE_CORE_ASSERT(_type == Type::Array, "Trying to index into non-array element");
	const auto& data = static_cast<DataExtension::Array&>(*_dataExtension);
	SE_CORE_ASSERT(index < data.Size);
	return {offset + data.ElementSize * index, &data.Element};
}

String LayoutElement::GetStructSignature() const
{
	using namespace std::string_literals;
	auto sig = "St{"s;
	for (const auto& [key, value] : static_cast<DataExtension::Struct&>(*_dataExtension).Elements)
	{
		sig += key + ":"s + value.GetSignature() + ";"s;
	}
	sig += "}"s;
	return sig;
}

String LayoutElement::GetArraySignature() const
{
	using namespace std::string_literals;
	const auto& data = static_cast<DataExtension::Array&>(*_dataExtension);
	return "Ar:"s + std::to_string(data.Size) + "{"s + data.Element.GetSignature() + "}"s;
}

bool LayoutElement::ValidateName(const String& name)
{
	return !name.empty() && !std::isdigit(name.front()) && std::all_of(name.begin(), name.end(), [](char c)
	{
		return std::isalnum(c) || c == '_';
	});
}

bool LayoutElement::IsLeafType(Type type)
{
	SE_CORE_ASSERT(type != Type::Count);
	return static_cast<int>(type) < static_cast<int>(Type::Struct);
}

size_t LayoutElement::AdvanceIfCrossesBoundary(size_t offset, size_t size)
{
	return CrossesBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
}

size_t LayoutElement::AdvanceToBoundary(size_t offset)
{
	return offset + (16u - offset % 16u) % 16u;
}

bool LayoutElement::CrossesBoundary(size_t offset, size_t size)
{
	const auto end = offset + size;
	const auto pageStart = offset / 16u;
	const auto pageEnd = end / 16u;

	return pageStart != pageEnd && end % 16 != 0u || size > 16u;
}

size_t Layout::GetSize() const
{
	return _root->GetSize();
}

String Layout::GetSignature() const
{
	return _root->GetSignature();
}

Layout::Layout(Shared<LayoutElement> root) :
	_root(Move(root))
{
}

EditableLayout::EditableLayout() :
	Layout(CreateSharedFromDonor(new LayoutElement(Type::Struct)))
{
}

LayoutElement& EditableLayout::operator[](const String& key)
{
	return (*_root)[key];
}

const LayoutElement& EditableLayout::operator[](const String& key) const
{
	return const_cast<EditableLayout&>(*this)[key];
}

void EditableLayout::ClearRoot()
{
	_root = CreateSharedFromDonor(new LayoutElement(Type::Struct));
}

Shared<LayoutElement> EditableLayout::DeliverRoot()
{
	auto preparedRoot = Move(_root);
	preparedRoot->Finalize(0);
	ClearRoot();
	return Move(preparedRoot);
}

const LayoutElement& ReadonlyLayout::operator[](const String& key) const
{
	return (*_root)[key];
}

const Shared<LayoutElement>& ReadonlyLayout::ShareRoot() const
{
	return _root;
}

Shared<LayoutElement> ReadonlyLayout::PilferRoot() const
{
	return Move(_root);
}

ReadonlyLayout::ReadonlyLayout(Shared<LayoutElement> root) :
	_root(Move(root))
{
}

ConstElementRef::Ptr::Ptr(const ConstElementRef* ref) :
	_ref(ref)
{
}

ConstElementRef::Ptr ConstElementRef::operator&() const
{
	return Ptr{const_cast<ConstElementRef*>(this)};
}

ConstElementRef ConstElementRef::operator[](const String& key) const
{
	return ConstElementRef{&(*_layoutElement)[key], _data, _offset};
}

ConstElementRef ConstElementRef::operator[](size_t index) const
{
	const auto [offset, element] = _layoutElement->CalculateIndexOffset(_offset, index);
	return ConstElementRef{element, _data, offset};
}

bool ConstElementRef::Exists() const
{
	return _layoutElement->Exists();
}

ConstElementRef::ConstElementRef(const LayoutElement* layoutElement, const char* data, size_t offset) :
	_layoutElement(layoutElement),
	_data(data),
	_offset(offset)
{
}

ElementRef::Ptr::Ptr(ElementRef* ref) :
	_ref(ref)
{
}

ElementRef::operator ConstElementRef() const
{
	return {_layoutElement, _data, _offset};
}

ElementRef::Ptr ElementRef::operator&() const
{
	return Ptr{const_cast<ElementRef*>(this)};
}

ElementRef ElementRef::operator[](const String& key) const
{
	return ElementRef{&(*_layoutElement)[key], _data, _offset};
}

ElementRef ElementRef::operator[](size_t index) const
{
	const auto [offset, element] = _layoutElement->CalculateIndexOffset(_offset, index);
	return ElementRef{element, _data, offset};
}

ElementRef::ElementRef(const LayoutElement* layoutElement, char* data, size_t offset) :
	_layoutElement(layoutElement),
	_data(data),
	_offset(offset)
{
}

ReadonlyLayout LayoutCodex::Resolve(EditableLayout&& layout)
{
	const auto signature = layout.GetSignature();
	auto& map = Instance()._map;

	const auto tryFind = map.find(signature);
	if (tryFind != map.end())
	{
		layout.ClearRoot();
		return ReadonlyLayout{tryFind->second};
	}
	const auto [value, didInsert] = map.emplace(Move(signature), layout.DeliverRoot());
	return ReadonlyLayout{value->second};
}

LayoutCodex& LayoutCodex::Instance() noexcept
{
	static LayoutCodex codex;
	return codex;
}

Buffer::Buffer(EditableLayout&& layout) :
	Buffer(LayoutCodex::Resolve(Move(layout)))
{
}

Buffer::Buffer(const ReadonlyLayout& layout) :
	_root(layout.ShareRoot()),
	_data(_root->GetOffsetAfter())
{
}

Buffer::Buffer(ReadonlyLayout&& layout) noexcept :
	_root(layout.PilferRoot()),
	_data(_root->GetOffsetAfter())
{
}

Buffer::Buffer(const Buffer& buffer) :
	_root(buffer._root),
	_data(buffer._data)
{
}

Buffer::Buffer(Buffer&& buffer) noexcept :
	_root(Move(buffer._root)),
	_data(Move(buffer._data))
{
}

Buffer& Buffer::operator=(const Buffer& buffer)
{
	_root = buffer._root;
	_data = buffer._data;
	return *this;
}

ElementRef Buffer::operator[](const String& key)
{
	return {&(*_root)[key], _data.data(), 0u};
}

ConstElementRef Buffer::operator[](const String& key) const
{
	return const_cast<Buffer&>(*this)[key];
}

const char* Buffer::GetData() const
{
	return _data.data();
}

size_t Buffer::GetSize() const
{
	return _data.size();
}

const LayoutElement& Buffer::GetRoot() const
{
	return *_root;
}

Shared<LayoutElement> Buffer::ShareLayoutRoot() const
{
	return _root;
}
}
