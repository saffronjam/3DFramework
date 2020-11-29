#pragma once

#include <unordered_set>

#include "Saffron/Base.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{

////////////////////////////////////////////////////////////////
/// Material
////////////////////////////////////////////////////////////////

class Material : public Resource
{
	friend class MaterialInstance;
public:
	enum class Flag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2),
		TwoSided = BIT(3)
	};

public:
	Material(const std::shared_ptr<Shader> &shader);
	virtual ~Material() = default;

	void Bind();

	Uint32 GetFlags() const { return m_MaterialFlags; }
	template<typename T>
	T &Get(const String &name);
	template<typename T>
	std::shared_ptr<T> GetResource(const String &name);

	void SetFlag(Flag flag) { m_MaterialFlags |= static_cast<Uint32>(flag); }
	template <typename T>
	void Set(const String &name, const T &value);
	void Set(const String &name, const std::shared_ptr<Texture> &texture);
	void Set(const String &name, const std::shared_ptr<Texture2D> &texture);
	void Set(const String &name, const std::shared_ptr<TextureCube> &texture);

	ShaderResourceDeclaration *FindResourceDeclaration(const String &name);

	static std::shared_ptr<Material> Create(const std::shared_ptr<Shader> &shader);

private:
	void AllocateStorage();
	void OnShaderReloaded();
	void BindTextures();

	ShaderUniformDeclaration *FindUniformDeclaration(const String &name);
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);
private:
	std::shared_ptr<Shader> m_Shader;
	std::unordered_set<MaterialInstance *> m_MaterialInstances;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	ArrayList<std::shared_ptr<Texture>> m_Textures;

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
std::shared_ptr<T> Material::GetResource(const String &name)
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

class MaterialInstance : public MemManaged<MaterialInstance>
{
	friend class Material;
public:
	MaterialInstance(const std::shared_ptr<Material> &material, String name = "");
	virtual ~MaterialInstance();

	void Bind();

	const String &GetName() const { return m_Name; }
	std::shared_ptr<Shader> GetShader() { return m_Material->m_Shader; }
	Uint32 GetFlags() const { return m_Material->m_MaterialFlags; }
	bool GetFlag(Material::Flag flag) const { return static_cast<Uint32>(flag) & m_Material->m_MaterialFlags; }
	template<typename T>
	T &Get(const String &name);
	template<typename T>
	std::shared_ptr<T> GetResource(const String &name);
	template<typename T>
	std::shared_ptr<T> TryGetResource(const String &name);

	void SetFlag(Material::Flag flag, bool value = true);
	template <typename T>
	void Set(const String &name, const T &value);
	void Set(const String &name, const std::shared_ptr<Texture> &texture);
	void Set(const String &name, const std::shared_ptr<Texture2D> &texture);
	void Set(const String &name, const std::shared_ptr<TextureCube> &texture);

public:
	static std::shared_ptr<MaterialInstance> Create(const std::shared_ptr<Material> &material);
private:
	void AllocateStorage();
	void OnShaderReloaded();
	Buffer &GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration);
	void OnMaterialValueUpdated(ShaderUniformDeclaration *decl);
private:
	std::shared_ptr<Material> m_Material;
	String m_Name;

	Buffer m_VSUniformStorageBuffer;
	Buffer m_PSUniformStorageBuffer;
	ArrayList<std::shared_ptr<Texture>> m_Textures;

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
std::shared_ptr<T> MaterialInstance::GetResource(const String &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	SE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
	const Uint32 slot = decl->GetRegister();
	SE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
	return std::shared_ptr<T>(m_Textures[slot]);
}

template <typename T>
std::shared_ptr<T> MaterialInstance::TryGetResource(const String &name)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	if ( !decl )
		return nullptr;

	const Uint32 slot = decl->GetRegister();
	if ( slot >= m_Textures.size() )
		return nullptr;

	return std::static_pointer_cast<T>(m_Textures[slot]);
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