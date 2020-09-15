#pragma once

#include <unordered_set>

#include "Saffron/Config.h"
#include "Saffron/Core/Ref.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/System/Macros.h"

namespace Se
{
////////////////////////////////////////////////////////////////
/// Material
////////////////////////////////////////////////////////////////

class Material : public RefCounted
{
	friend class MaterialInstance;
public:
	enum class Flag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2)
	};

public:
	explicit Material(const Ref<Shader> &shader);
	virtual ~Material() = default;

	void Bind();

	Uint32 GetFlags() const { return m_MaterialFlags; }
	template<typename T>
	T &Get(const std::string &name);
	template<typename T>
	Ref<T> GetResource(const std::string &name);

	void SetFlag(Flag flag) { m_MaterialFlags |= static_cast<Uint32>(flag); }
	template<typename T>
	void Set(const std::string &name, const T &value);
	void Set(const std::string &name, const Ref<Texture> &texture);
	void Set(const std::string &name, const Ref<Texture2D> &texture);
	void Set(const std::string &name, const Ref<TextureCube> &texture);


	static Ref<Material> Create(const Ref<Shader> &shader);

private:
	void AllocateStorage();
	void OnShaderReloaded();
	void BindTextures();

	ShaderUniformDeclaration *FindUniformDeclaration(const std::string &name);
	ShaderResourceDeclaration *FindResourceDeclaration(const std::string &name);
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);

private:
	Ref<Shader> m_Shader;
	std::unordered_set<MaterialInstance *> m_MaterialInstances;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	std::vector<Ref<Texture>> m_Textures;

	Uint32 m_MaterialFlags;
};




////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

class MaterialInstance : public RefCounted
{
	friend class Material;
public:
	MaterialInstance(const Ref<Material> &material, std::string name = "");
	virtual ~MaterialInstance();

	void Bind();

	const std::string &GetName() const { return m_Name; }
	Ref<Shader> GetShader() { return m_Material->m_Shader; }
	Uint32 GetFlags() const { return m_Material->m_MaterialFlags; }
	bool GetFlag(Material::Flag flag) const { return static_cast<Uint32>(flag) & m_Material->m_MaterialFlags; }
	template<typename T>
	T &Get(const std::string &name);
	template<typename T>
	Ref<T> GetResource(const std::string &name);
	template<typename T>
	Ref<T> TryGetResource(const std::string &name);

	void SetFlag(Material::Flag flag, bool value = true);
	template <typename T>
	void Set(const std::string &name, const T &value);
	void Set(const std::string &name, const Ref<Texture> &texture);
	void Set(const std::string &name, const Ref<Texture2D> &texture);
	void Set(const std::string &name, const Ref<TextureCube> &texture);

public:
	static Ref<MaterialInstance> Create(const Ref<Material> &material);
private:
	void AllocateStorage();
	void OnShaderReloaded();
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);
	void OnMaterialValueUpdated(ShaderUniformDeclaration *decl);
private:
	Ref<Material> m_Material;
	std::string m_Name;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	std::vector<Ref<Texture>> m_Textures;

	// TODO: This is temporary; come up with a proper system to track overrides
	std::unordered_set<std::string> m_OverriddenValues;
};




////////////////////////////////////////////////////////////////
/// Material
////////////////////////////////////////////////////////////////

template <typename T>
T &Material::Get(const std::string &name)
{
	auto *decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Ref<T> Material::GetResource(const std::string &name)
{
	const auto *decl = FindResourceDeclaration(name);
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return m_Textures[slot];
}

template <typename T>
void Material::Set(const std::string &name, const T &value)
{
	const auto decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);
	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	for ( auto mi : m_MaterialInstances )
		mi->OnMaterialValueUpdated(decl);
}




////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

template <typename T>
T &MaterialInstance::Get(const std::string &name)
{
	auto *decl = m_Material->FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Ref<T> MaterialInstance::GetResource(const std::string &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return Ref<T>(m_Textures[slot]);
}

template <typename T>
Ref<T> MaterialInstance::TryGetResource(const std::string &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	if ( !decl )
		return nullptr;

	const Uint32 slot = decl->GetRegister();
	if ( slot >= m_Textures.size() )
		return nullptr;

	return Ref<T>(m_Textures[slot]);
}

template <typename T>
void MaterialInstance::Set(const std::string &name, const T &value)
{
	auto *decl = m_Material->FindUniformDeclaration(name);
	if ( !decl )
		return;

	//TODO: Fix so I can exchange 'x' with $name
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);

	buffer.Write(&value, decl->GetSize(), decl->GetOffset());

	m_OverriddenValues.insert(name);
}

inline void Material::Set(const std::string &name, const Ref<Texture> &texture)
{
	const auto *decl = FindResourceDeclaration(name);
	const Uint32 slot = decl->GetRegister();
	if ( m_Textures.size() <= slot )
		m_Textures.resize(static_cast<size_t>(slot) + 1);
	m_Textures[slot] = texture;
}




}

