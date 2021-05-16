#include "SaffronPCH.h"

#include "Saffron/Script/ScriptEntity.h"
#include "Saffron/Script/Field.h"

namespace Se
{
#define ELEMENTS_FN \
	X( Mono::FieldType::Float ) \
	X( Mono::FieldType::Int ) \
	X( Mono::FieldType::Uint ) \
	X( Mono::FieldType::String ) \
	X( Mono::FieldType::Vec2 ) \
	X( Mono::FieldType::Vec3 ) \
	X( Mono::FieldType::Vec4 )

Field::Field(const ScriptEntity& parent, Mono::Field field) :
	_parent(parent),
	_monoField(Move(field))
{
	AllocateBuffer(_monoField.GetFieldType());
}

Field::Field(Field&& other) noexcept :
	_parent(other._parent),
	_monoField(Move(other._monoField)),
	_storedValueBuffer(other._storedValueBuffer)
{
	other._storedValueBuffer.Reset();
}

Field::~Field()
{
	_storedValueBuffer.Destroy();
}

void Field::CopyStoredValueToRuntime() const
{
	Debug::Assert(_parent.GetMonoObject().Valid(), "Mono object was not valid when copying stored value to runtime");
	switch (_monoField.GetFieldType())
	{
	case Mono::FieldType::Float:
	{
		using Type = Map<Mono::FieldType::Float>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::Int:
	{
		using Type = Map<Mono::FieldType::Int>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::Uint:
	{
		using Type = Map<Mono::FieldType::Uint>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::String:
	{
		using Type = Map<Mono::FieldType::String>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::Vec2:
	{
		using Type = Map<Mono::FieldType::Vec2>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::Vec3:
	{
		using Type = Map<Mono::FieldType::Vec3>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	}
	case Mono::FieldType::Vec4:
	{
		using Type = Map<Mono::FieldType::Vec4>::SysType;
		const auto invoker = GetFieldInvoker<Type>(_monoField);
		invoker.SetValue(_parent.GetMonoObject(),
		                 *reinterpret_cast<Type*>(const_cast<uchar*>(_storedValueBuffer.Data())));
		break;
	};
	case Mono::FieldType::None:
	{
		Debug::Break("Tried to copy invalid field type. Field: \"{0}\"", _monoField.GetFullDeclname());
		break;
	}
	default:
	{
		Debug::Break("Tried to copy invalid field type. Field: \"{0}\"", _monoField.GetFullDeclname());
		break;
	}
	}
}

auto Field::IsRuntimeAvailable() const -> bool
{
	return _parent.GetMonoObject().Valid();
}

void Field::SetStoredValue(void* src) const
{
	const uint size = GetSize(_monoField.GetFieldType());
	memcpy(const_cast<uchar*>(_storedValueBuffer.Data()), src, size);
}

auto Field::GetMonoField() const -> const Mono::Field&
{
	return _monoField;
}

auto Field::GetSize(Mono::FieldType type) -> uint
{
	return Bridge<Functors::GetSize>(type);
}

auto Field::GetString(Mono::FieldType type) -> String
{
	return Bridge<Functors::GetString>(type);
}

void Field::AllocateBuffer(Mono::FieldType type)
{
	const uint size = GetSize(type);
	_storedValueBuffer = Buffer(new uchar[size], size);
	_storedValueBuffer.ZeroInitialize();
}

void Field::GetStoredValue_Internal(void* outValue) const
{
	const uint size = GetSize(_monoField.GetFieldType());
	memcpy(outValue, _storedValueBuffer.Data(), size);
}

void Field::SetStoredValue_Internal(void* value) const
{
	const uint size = GetSize(_monoField.GetFieldType());
	memcpy(const_cast<uchar*>(_storedValueBuffer.Data()), value, size);
}

#undef ELEMENTS_FN
}
