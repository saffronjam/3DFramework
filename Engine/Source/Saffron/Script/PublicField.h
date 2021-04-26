﻿#pragma once

#include "Saffron/Base.h"
#include "Saffron/Script/MonoDeclarations.h"

namespace Se
{
enum class FieldType
{
	None = 0,
	Float,
	Int,
	Uint,
	String,
	Vec2,
	Vec3,
	Vec4
};

struct PublicField
{
	String Name;
	FieldType Type;

	PublicField(String name, FieldType type);
	PublicField(const PublicField&) = delete;
	PublicField(PublicField&& other) noexcept;
	~PublicField();

	void CopyStoredValueToRuntime() const;
	bool IsRuntimeAvailable() const;

	template <typename T>
	T GetStoredValue() const;
	template <typename T>
	T GetRuntimeValue() const;

	template <typename T>
	void SetStoredValue(T value) const;
	template <typename T>
	void SetRuntimeValue(T value) const;
	void SetStoredValue(void* src) const;

	static uint GetSize(FieldType type);
	static String GetString(FieldType type);

public:
	struct EntityInstance* _entityInstance{};
	MonoClassField* _monoClassField{};
	uchar* _storedValueBuffer = nullptr;

	uchar* AllocateBuffer(FieldType type);
	void SetStoredValue_Internal(void* value) const;
	void GetStoredValue_Internal(void* outValue) const;
	void SetRuntimeValue_Internal(void* value) const;
	void GetRuntimeValue_Internal(void* outValue) const;

	friend class ScriptEngine;
};

template <typename T>
T PublicField::GetStoredValue() const
{
	T value;
	GetStoredValue_Internal(&value);
	return value;
}

template <typename T>
T PublicField::GetRuntimeValue() const
{
	T value;
	GetRuntimeValue_Internal(&value);
	return value;
}

template <typename T>
void PublicField::SetStoredValue(T value) const
{
	SetStoredValue_Internal(&value);
}

template <typename T>
void PublicField::SetRuntimeValue(T value) const
{
	SetRuntimeValue_Internal(&value);
}
}
