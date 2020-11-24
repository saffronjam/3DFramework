#pragma once

#include <unordered_set>

#include "Saffron/Base.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{

////////////////////////////////////////////////////////////////
/// Material
////////////////////////////////////////////////////////////////

class Material : public ReferenceCounted
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
	Material(const Shared<Shader> &shader);
	virtual ~Material() = default;

	void Bind();

	Uint32 GetFlags() const { return m_MaterialFlags; }
	template<typename T>
	T &Get(const String &name);
	template<typename T>
	Shared<T> GetResource(const String &name);

	void SetFlag(Flag flag) { m_MaterialFlags |= static_cast<Uint32>(flag); }
	template <typename T>
	void Set(const String &name, const T &value);
	void Set(const String &name, const Shared<Texture> &texture);
	void Set(const String &name, const Shared<Texture2D> &texture);
	void Set(const String &name, const Shared<TextureCube> &texture);

	static Shared<Material> Create(const Shared<Shader> &shader);

private:
	void AllocateStorage();
	void OnShaderReloaded();
	void BindTextures();

	ShaderUniformDeclaration *FindUniformDeclaration(const String &name);
	ShaderResourceDeclaration *FindResourceDeclaration(const String &name);
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);
private:
	Shared<Shader> m_Shader;
	std::unordered_set<MaterialInstance *> m_MaterialInstances;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	ArrayList<Shared<Texture>> m_Textures;

	Uint32 m_MaterialFlags{};
};

template <typename T>
T &Material::Get(const String &name)
{
	auto decl = FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Shared<T> Material::GetResource(const String &name)
{
	auto decl = FindResourceDeclaration(name);
	Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return m_Textures[slot];
}

template <typename T>
void Material::Set(const String &name, const T &value)
{
	Renderer::Submit([=]()
					 {
						 {
							 auto decl = FindUniformDeclaration(name);
							 SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
							 auto &buffer = GetUniformBufferTarget(decl);
							 buffer.Write(&value, decl->GetSize(), decl->GetOffset());

							 for ( auto mi : m_MaterialInstances )
								 mi->OnMaterialValueUpdated(decl);
						 }
					 });

}







////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

class MaterialInstance : public ReferenceCounted
{
	friend class Material;
public:
	MaterialInstance(const Shared<Material> &material, String name = "");
	virtual ~MaterialInstance();

	void Bind();

	const String &GetName() const { return m_Name; }
	Shared<Shader> GetShader() { return m_Material->m_Shader; }
	Uint32 GetFlags() const { return m_Material->m_MaterialFlags; }
	bool GetFlag(Material::Flag flag) const { return static_cast<Uint32>(flag) & m_Material->m_MaterialFlags; }
	template<typename T>
	T &Get(const String &name);
	template<typename T>
	Shared<T> GetResource(const String &name);
	template<typename T>
	Shared<T> TryGetResource(const String &name);

	void SetFlag(Material::Flag flag, bool value = true);
	template <typename T>
	void Set(const String &name, const T &value);
	void Set(const String &name, const Shared<Texture> &texture);
	void Set(const String &name, const Shared<Texture2D> &texture);
	void Set(const String &name, const Shared<TextureCube> &texture);

public:
	static Shared<MaterialInstance> Create(const Shared<Material> &material);
private:
	void AllocateStorage();
	void OnShaderReloaded();
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);
	void OnMaterialValueUpdated(ShaderUniformDeclaration *decl);
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
T &MaterialInstance::Get(const String &name)
{
	auto *decl = m_Material->FindUniformDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	auto &buffer = GetUniformBufferTarget(decl);
	return buffer.Read<T>(decl->GetOffset());
}

template <typename T>
Shared<T> MaterialInstance::GetResource(const String &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return Shared<T>(m_Textures[slot]);
}

template <typename T>
Shared<T> MaterialInstance::TryGetResource(const String &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	if ( !decl )
		return nullptr;

	const Uint32 slot = decl->GetRegister();
	if ( slot >= m_Textures.size() )
		return nullptr;

	return Shared<T>(m_Textures[slot]);
}

template <typename T>
void MaterialInstance::Set(const String &name, const T &value)
{
	Renderer::Submit([=]()
					 {
						 auto *decl = m_Material->FindUniformDeclaration(name);
						 if ( !decl )
							 return;

						 //TODO: Fix so I can exchange 'x' with $name
						 SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
						 auto &buffer = GetUniformBufferTarget(decl);

						 buffer.Write(&value, decl->GetSize(), decl->GetOffset());

						 m_OverriddenValues.insert(name);
					 });
}
}