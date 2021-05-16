#pragma once

#include <monopp/Field.h>
#include <monopp/FieldInvoker.h>

#include <any>
#include "Saffron/Core/Buffer.h"

namespace Mono
{
template <>
struct ConvertMonoType<Se::Vector2>
{
	using CppType = Se::Vector2;
	using MonoUnboxedType = Se::Vector2;
	using MonoBoxed = MonoObject*;

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		return obj;
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return obj;
	}

	static auto from_mono_boxed(const MonoBoxed& obj) -> CppType
	{
		return *reinterpret_cast<Se::Vector2*>(obj);
	}
};

template <>
struct ConvertMonoType<Se::Vector3>
{
	using CppType = Se::Vector3;
	using MonoUnboxedType = Se::Vector3;
	using MonoBoxed = MonoObject*;

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		return obj;
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return obj;
	}

	static auto from_mono_boxed(const MonoBoxed& obj) -> CppType
	{
		return *reinterpret_cast<Se::Vector3*>(obj);
	}
};

template <>
struct ConvertMonoType<Se::Vector4>
{
	using CppType = Se::Vector4;
	using MonoUnboxedType = Se::Vector4;
	using MonoBoxed = MonoObject*;

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		return obj;
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return obj;
	}

	static auto from_mono_boxed(const MonoBoxed& obj) -> CppType
	{
		return *reinterpret_cast<Se::Vector4*>(obj);
	}
};
}


namespace Se
{
class ScriptEntity;

#pragma region Maps
template <Mono::FieldType type>
struct Map
{
	static constexpr bool Valid = false;
};


template <>
struct Map<Mono::FieldType::None>
{
	using SysType = int;
	static constexpr size_t Size = 0;
	static constexpr const char* Code = "None";
	static constexpr bool Valid = false;
};

template <>
struct Map<Mono::FieldType::Float>
{
	using SysType = float;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Float";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::Int>
{
	using SysType = int;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Int";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::Uint>
{
	using SysType = uint;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Uint";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::String>
{
	using SysType = String;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "String";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::Vec2>
{
	using SysType = Vector2;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Vec2";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::Vec3>
{
	using SysType = Vector3;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Vec3";
	static constexpr bool Valid = true;
};

template <>
struct Map<Mono::FieldType::Vec4>
{
	using SysType = Vector4;
	static constexpr size_t Size = sizeof(SysType);
	static constexpr const char* Code = "Vec4";
	static constexpr bool Valid = true;
};
#pragma endregion

class Field
{
private:
	struct Functors
	{
		template <Mono::FieldType fieldType>
		struct CreateInvoker
		{
			static constexpr auto Exec(const Mono::Field& field)
			{
				return std::any(Mono::CreateFieldInvoker<typename Map<fieldType>::SysType>(field));
			}
		};

		template <Mono::FieldType fieldType>
		struct GetString
		{
			static constexpr auto Exec()
			{
				return Map<fieldType>::Code;
			}
		};

		template <Mono::FieldType fieldType>
		struct GetSize
		{
			static constexpr auto Exec()
			{
				return Map<fieldType>::Size;
			}
		};

		template <Mono::FieldType fieldType>
		struct GetMap
		{
			static constexpr auto Exec()
			{
				return Map<fieldType>();
			}
		};
	};

public:
	Field(const ScriptEntity& parent, Mono::Field field);
	Field(const Field&) = delete;
	Field(Field&& other) noexcept;
	~Field();

	void CopyStoredValueToRuntime() const;
	auto IsRuntimeAvailable() const -> bool;

	template <typename T>
	auto GetStoredValue() const -> T;
	template <typename T>
	auto GetRuntimeValue() const -> T;

	template <typename T>
	void SetStoredValue(T value) const;
	template <typename T>
	void SetRuntimeValue(T value) const;
	void SetStoredValue(void* src) const;

	auto GetMonoField() const -> const Mono::Field&;

	static auto GetSize(Mono::FieldType type) -> uint;
	static auto GetString(Mono::FieldType type) -> String;

	template <typename T>
	auto GetFieldInvoker(const Mono::Field& field) const
	{
		return std::any_cast<Mono::FieldInvoker<T>>(Bridge<Functors::CreateInvoker>(_monoField.GetFieldType(), field));
	}

private:
	template <template<Mono::FieldType> class F, typename... Args>
	static constexpr auto Bridge(Mono::FieldType type, Args&&... args)
	{
		switch (type)
		{
		case Mono::FieldType::None: return F<Mono::FieldType::None>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Float: return F<Mono::FieldType::Float>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Int: return F<Mono::FieldType::Int>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Uint: return F<Mono::FieldType::Uint>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::String: return F<Mono::FieldType::String>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Vec2: return F<Mono::FieldType::Vec2>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Vec3: return F<Mono::FieldType::Vec3>::Exec(std::forward<Args>(args)...);
		case Mono::FieldType::Vec4: return F<Mono::FieldType::Vec4>::Exec(std::forward<Args>(args)...);;
		default:
		{
			Debug::Break("Invalid field type when bridging");
			return F<Mono::FieldType::Int>::Exec(std::forward<Args>(args)...);
		}
		}
	}

private:
	const ScriptEntity& _parent;
	Mono::Field _monoField;
	Buffer _storedValueBuffer;

	void AllocateBuffer(Mono::FieldType type);
	void SetStoredValue_Internal(void* value) const;
	void GetStoredValue_Internal(void* outValue) const;

	friend class ScriptEngine;
};

template <typename T>
auto Field::GetStoredValue() const -> T
{
	T value;
	GetStoredValue_Internal(&value);
	return value;
}

template <typename T>
void Field::SetStoredValue(T value) const
{
	SetStoredValue_Internal(&value);
}

template <typename T>
auto Field::GetRuntimeValue() const -> T
{
	auto invoker = GetFieldInvoker<T>(_monoField);
	return invoker.GetValue(_parent.GetMonoObject());
}

template <typename T>
void Field::SetRuntimeValue(T value) const
{
	auto invoker = GetFieldInvoker<T>(_monoField);
	invoker.SetValue(_parent.GetMonoObject(), value);
}
}
