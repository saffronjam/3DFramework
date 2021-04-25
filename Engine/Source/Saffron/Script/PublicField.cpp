#include "SaffronPCH.h"

#include "Saffron/Script/EntityScriptClass.h"
#include "Saffron/Script/PublicField.h"

namespace Se
{
PublicField::PublicField(String name, FieldType type) :
	Name(Move(name)),
	Type(type)
{
	_storedValueBuffer = AllocateBuffer(type);
}

PublicField::PublicField(PublicField&& other) noexcept
{
	Name = Move(other.Name);
	Type = other.Type;
	_entityInstance = other._entityInstance;
	_monoClassField = other._monoClassField;
	_storedValueBuffer = other._storedValueBuffer;

	other._entityInstance = nullptr;
	other._monoClassField = nullptr;
	other._storedValueBuffer = nullptr;
}

PublicField::~PublicField()
{
	delete[] _storedValueBuffer;
}

void PublicField::CopyStoredValueToRuntime() const
{
	Debug::Assert(_entityInstance->GetInstance());;
	MonoApi::SetFieldValue(_entityInstance->GetInstance(), _monoClassField, _storedValueBuffer);
}

bool PublicField::IsRuntimeAvailable() const
{
	return _entityInstance->Handle != 0;
}

void PublicField::SetStoredValue(void* src) const
{
	const uint size = GetSize(Type);
	memcpy(_storedValueBuffer, src, size);
}

uint PublicField::GetSize(FieldType type)
{
	switch (type)
	{
	case FieldType::Float:
	case FieldType::Int:
	case FieldType::Uint: return 4;
		// case FieldType::String:   return 8; // TODO
	case FieldType::Vec2: return 4 * 2;
	case FieldType::Vec3: return 4 * 3;
	case FieldType::Vec4: return 4 * 4;
	default: Debug::Break("Unknown field type");
		return 0;
	}
}

String PublicField::GetString(FieldType type)
{
	switch (type)
	{
	case FieldType::Float: return "Float";
	case FieldType::Int: return "Int";
	case FieldType::Uint: return "UnsignedInt";
	case FieldType::String: return "String";
	case FieldType::Vec2: return "Vec2";
	case FieldType::Vec3: return "Vec3";
	case FieldType::Vec4: return "Vec4";
	default: return "Unknown";
	}
}

uchar* PublicField::AllocateBuffer(FieldType type)
{
	const uint size = GetSize(type);
	auto* buffer = new uchar[size];
	memset(buffer, 0, size);
	return buffer;
}

void PublicField::GetStoredValue_Internal(void* outValue) const
{
	const uint size = GetSize(Type);
	memcpy(outValue, _storedValueBuffer, size);
}

void PublicField::GetRuntimeValue_Internal(void* outValue) const
{
	Debug::Assert(_entityInstance->GetInstance());;
	MonoApi::SetFieldValue(_entityInstance->GetInstance(), _monoClassField, outValue);
}

void PublicField::SetStoredValue_Internal(void* value) const
{
	const uint size = GetSize(Type);
	memcpy(_storedValueBuffer, value, size);
}

void PublicField::SetRuntimeValue_Internal(void* value) const
{
	Debug::Assert(_entityInstance->GetInstance());;
	MonoApi::SetFieldValue(_entityInstance->GetInstance(), _monoClassField, value);
}
}
