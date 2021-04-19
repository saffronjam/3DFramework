#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
////////////////////////////////////////////////////////////////
/// Material
////////////////////////////////////////////////////////////////

enum class MaterialFlag
{
	None = BIT(0),
	DepthTest = BIT(1),
	Blend = BIT(2),
	TwoSided = BIT(3)
};

class Material : public Managed
{
	friend class MaterialInstance;
public:
	explicit Material(const Shared<Shader>& shader);
	virtual ~Material() = default;

	void Bind();

	Uint32 GetFlags() const;
	void SetFlag(MaterialFlag flag);

	const Shared<Shader>& GetShader() const;

	template <typename T>
	T& Get(const String& name);
	template <typename T>
	Shared<T> GetResource(const String& name);
	template <typename T>
	void Set(const String& name, const T& value);
	void Set(const String& name, const Shared<Texture>& texture);
	void Set(const String& name, const Shared<Texture2D>& texture);
	void Set(const String& name, const Shared<TextureCube>& texture);

	ShaderResourceDeclaration* FindResourceDeclaration(const String& name);

	static Shared<Material> Create(const Shared<Shader>& shader);

private:
	void AllocateStorage();
	bool OnShaderReloaded();
	void BindTextures();

	ShaderUniformDeclaration* FindUniformDeclaration(const String& name);
	Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
private:
	Shared<Shader> _shader;
	UnorderedSet<MaterialInstance*> _materialInstances;

	Buffer _vSUniformStorageBuffer;
	Buffer _pSUniformStorageBuffer;
	ArrayList<Shared<Texture>> _textures;

	Uint32 _materialFlags{};
};

template <typename T>
T& Material::Get(const String& name)
{
	auto decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Shared<T> Material::GetResource(const String& name)
{
	auto decl = FindResourceDeclaration(name);
	Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < _textures.size(), "Texture slot is invalid!");
	return _textures[slot];
}

template <typename T>
void Material::Set(const String& name, const T& value)
{
	auto decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);
	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	for (auto* mi : _materialInstances) mi->OnMaterialValueUpdated(decl);
}


////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

class MaterialInstance : public Managed
{
	friend class Material;
public:
	MaterialInstance(const Shared<Material>& material, String name = "");
	virtual ~MaterialInstance();

	void Bind();

	const String& GetName() const;
	const Shared<Shader>& GetShader() const;
	Uint32 GetFlags() const;
	bool GetFlag(MaterialFlag flag) const;

	template <typename T>
	T& Get(const String& name);
	template <typename T>
	Shared<T> GetResource(const String& name);
	template <typename T>
	Shared<T> TryGetResource(const String& name);

	void SetFlag(MaterialFlag flag, bool value = true);
	template <typename T>
	void Set(const String& name, const T& value);
	void Set(const String& name, const Shared<Texture>& texture);
	void Set(const String& name, const Shared<Texture2D>& texture);
	void Set(const String& name, const Shared<TextureCube>& texture);

public:
	static Shared<MaterialInstance> Create(const Shared<Material>& material);
private:
	void AllocateStorage();
	void OnShaderReloaded();
	Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
	void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);
private:
	Shared<Material> _material;
	String _name;

	Buffer _vSUniformStorageBuffer;
	Buffer _pSUniformStorageBuffer;
	ArrayList<Shared<Texture>> _textures;

	// TODO: This is temporary; come up with a proper system to track overrides
	std::unordered_set<String> _overriddenValues;
};


////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

template <typename T>
T& MaterialInstance::Get(const String& name)
{
	auto* decl = _material->FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Shared<T> MaterialInstance::GetResource(const String& name)
{
	const auto* decl = _material->FindResourceDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < _textures.size(), "Texture slot is invalid!");
	return Shared<T>(_textures[slot]);
}

template <typename T>
Shared<T> MaterialInstance::TryGetResource(const String& name)
{
	const auto* decl = _material->FindResourceDeclaration(name);
	if (!decl) return nullptr;

	const Uint32 slot = decl->GetRegister();
	if (slot >= _textures.size()) return nullptr;

	return _textures[slot];
}

template <typename T>
void MaterialInstance::Set(const String& name, const T& value)
{
	auto* decl = _material->FindUniformDeclaration(name);
	if (!decl) return;

	//TODO: Fix so I can exchange 'x' with $name
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);

	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	_overriddenValues.insert(name);
}
}
