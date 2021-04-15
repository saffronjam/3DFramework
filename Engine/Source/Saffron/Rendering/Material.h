#pragma once

#include <unordered_set>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Rendering/Renderer.h"
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
	Shared<Shader> m_Shader;
	std::unordered_set<MaterialInstance*> m_MaterialInstances;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	ArrayList<Shared<Texture>> m_Textures;

	Uint32 m_MaterialFlags{};
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
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return m_Textures[slot];
}

template <typename T>
void Material::Set(const String& name, const T& value)
{
	auto decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);
	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	for (auto* mi : m_MaterialInstances) mi->OnMaterialValueUpdated(decl);
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
	Shared<Shader> GetShader();
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
	Shared<Material> m_Material;
	String m_Name;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	ArrayList<Shared<Texture>> m_Textures;

	// TODO: This is temporary; come up with a proper system to track overrides
	std::unordered_set<String> m_OverriddenValues;
};


////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

template <typename T>
T& MaterialInstance::Get(const String& name)
{
	auto* decl = m_Material->FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Shared<T> MaterialInstance::GetResource(const String& name)
{
	const auto* decl = m_Material->FindResourceDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return Shared<T>(m_Textures[slot]);
}

template <typename T>
Shared<T> MaterialInstance::TryGetResource(const String& name)
{
	const auto* decl = m_Material->FindResourceDeclaration(name);
	if (!decl) return nullptr;

	const Uint32 slot = decl->GetRegister();
	if (slot >= m_Textures.size()) return nullptr;

	return m_Textures[slot];
}

template <typename T>
void MaterialInstance::Set(const String& name, const T& value)
{
	auto* decl = m_Material->FindUniformDeclaration(name);
	if (!decl) return;

	//TODO: Fix so I can exchange 'x' with $name
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto& buffer = GetUniformBufferTarget(decl);

	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	m_OverriddenValues.insert(name);
}
}
