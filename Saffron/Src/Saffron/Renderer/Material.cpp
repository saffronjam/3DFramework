#include "SaffronPCH.h"
#include "Material.h"

namespace Se {

//////////////////////////////////////////////////////////////////////////////////
// Material
//////////////////////////////////////////////////////////////////////////////////


Material::Material(const Shared<Shader> &shader)
	: m_Shader(shader)
{
	m_Shader->AddShaderReloadedCallback([this] { OnShaderReloaded(); });
	AllocateStorage();

	m_MaterialFlags |= static_cast<Uint32>(Flag::DepthTest);
	m_MaterialFlags |= static_cast<Uint32>(Flag::Blend);
}

void Material::Bind()
{
	m_Shader->Bind();

	if ( m_VSUniformStorageBuffer )
		m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

	if ( m_PSUniformStorageBuffer )
		m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

	BindTextures();
}

void Material::Set(const std::string &name, const Shared<Texture> &texture)
{
	ShaderResourceDeclaration *decl = FindResourceDeclaration(name);
	const Uint32 slot = decl->GetRegister();
	if ( m_Textures.size() <= slot )
		m_Textures.resize(static_cast<size_t>(slot) + 1);
	m_Textures[slot] = texture;
}

void Material::Set(const std::string &name, const Shared<Texture2D> &texture)
{
	Set(name, static_cast<const Shared<Texture> &>(texture));
}

void Material::Set(const std::string &name, const Shared<TextureCube> &texture)
{
	Set(name, static_cast<const Shared<Texture> &>(texture));
}

Shared<Material> Material::Create(const Shared<Shader> &shader)
{
	return Shared<Material>::Create(shader);
}

void Material::AllocateStorage()
{
	if ( m_Shader->HasVSMaterialUniformBuffer() )
	{
		const auto &vsBuffer = m_Shader->GetVSMaterialUniformBuffer();
		m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
		m_VSUniformStorageBuffer.ZeroInitialize();
	}

	if ( m_Shader->HasPSMaterialUniformBuffer() )
	{
		const auto &psBuffer = m_Shader->GetPSMaterialUniformBuffer();
		m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
		m_PSUniformStorageBuffer.ZeroInitialize();
	}
}

void Material::OnShaderReloaded()
{
	//TODO: WHY IS THIS HERE?!
	return;
	AllocateStorage();

	for ( auto &mi : m_MaterialInstances )
		mi->OnShaderReloaded();
}

void Material::BindTextures()
{
	Uint32 TextureIndex = 0;
	for ( auto &texture : m_Textures )
	{
		if ( texture )
			texture->Bind(TextureIndex);
		TextureIndex++;
	}
}

ShaderUniformDeclaration *Material::FindUniformDeclaration(const std::string &name)
{
	if ( m_VSUniformStorageBuffer )
	{
		const auto &declarations = m_Shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
		for ( ShaderUniformDeclaration *uniform : declarations )
		{
			if ( uniform->GetName() == name )
				return uniform;
		}
	}

	if ( m_PSUniformStorageBuffer )
	{
		const auto &declarations = m_Shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
		for ( ShaderUniformDeclaration *uniform : declarations )
		{
			if ( uniform->GetName() == name )
				return uniform;
		}
	}
	return nullptr;
}

ShaderResourceDeclaration *Material::FindResourceDeclaration(const std::string &name)
{
	const auto &resources = m_Shader->GetResources();
	for ( ShaderResourceDeclaration *resource : resources )
	{
		if ( resource->GetName() == name )
			return resource;
	}
	return nullptr;
}

Buffer &Material::GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration)
{
	switch ( uniformDeclaration->GetDomain() )
	{
	case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
	case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
	default:
		SE_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}
}




////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

MaterialInstance::MaterialInstance(const Shared<Material> &material, std::string name)
	: m_Material(material), m_Name(std::move(name))
{
	m_Material->m_MaterialInstances.insert(this);
	AllocateStorage();
}


MaterialInstance::~MaterialInstance()
{
	m_Material->m_MaterialInstances.erase(this);
}

void MaterialInstance::Bind()
{
	m_Material->m_Shader->Bind();

	if ( m_VSUniformStorageBuffer )
		m_Material->m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

	if ( m_PSUniformStorageBuffer )
		m_Material->m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

	m_Material->BindTextures();

	Uint32 TextureIndex = 0;
	for ( auto &texture : m_Textures )
	{
		if ( texture )
			texture->Bind(TextureIndex);
		TextureIndex++;
	}
}

void MaterialInstance::Set(const std::string &name, const Shared<Texture> &texture)
{
	const auto *decl = m_Material->FindResourceDeclaration(name);
	if ( !decl )
	{
		SE_CORE_WARN("Cannot find material property: ", name);
		return;
	}
	const Uint32 slot = decl->GetRegister();
	if ( m_Textures.size() <= slot )
		m_Textures.resize(static_cast<size_t>(slot) + 1);
	m_Textures[slot] = texture;
}

void MaterialInstance::Set(const std::string &name, const Shared<Texture2D> &texture)
{
	Set(name, static_cast<const Shared<Texture> &>(texture));
}

void MaterialInstance::Set(const std::string &name, const Shared<TextureCube> &texture)
{
	Set(name, static_cast<const Shared<Texture> &>(texture));
}

Shared<MaterialInstance> MaterialInstance::Create(const Shared<Material> &material)
{
	return Shared<MaterialInstance>::Create(material);
}

void MaterialInstance::AllocateStorage()
{
	if ( m_Material->m_Shader->HasVSMaterialUniformBuffer() )
	{
		const auto &vsBuffer = m_Material->m_Shader->GetVSMaterialUniformBuffer();
		m_VSUniformStorageBuffer = Buffer::Copy(m_Material->m_VSUniformStorageBuffer.Data(), vsBuffer.GetSize());
	}

	if ( m_Material->m_Shader->HasPSMaterialUniformBuffer() )
	{
		const auto &psBuffer = m_Material->m_Shader->GetPSMaterialUniformBuffer();
		m_PSUniformStorageBuffer = Buffer::Copy(m_Material->m_PSUniformStorageBuffer.Data(), psBuffer.GetSize());
	}
}

void MaterialInstance::OnShaderReloaded()
{
	AllocateStorage();
	m_OverriddenValues.clear();
}

Buffer &MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration *uniformDeclaration)
{
	switch ( uniformDeclaration->GetDomain() )
	{
	case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
	case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
	default:
		SE_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}

}

void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration *decl)
{
	if ( m_OverriddenValues.find(decl->GetName()) == m_OverriddenValues.end() )
	{
		auto &buffer = GetUniformBufferTarget(decl);
		auto &materialBuffer = m_Material->GetUniformBufferTarget(decl);
		buffer.Write(materialBuffer.Data() + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
	}
}

void MaterialInstance::SetFlag(Material::Flag flag, bool value)
{
	if ( value )
	{
		m_Material->m_MaterialFlags |= static_cast<Uint32>(flag);
	}
	else
	{
		m_Material->m_MaterialFlags &= ~static_cast<Uint32>(flag);
	}
}
}