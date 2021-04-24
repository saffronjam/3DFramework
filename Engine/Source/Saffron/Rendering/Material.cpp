#include "SaffronPCH.h"

#include "Saffron/Rendering/Material.h"

namespace Se
{
//////////////////////////////////////////////////////////////////////////////////
// Material
//////////////////////////////////////////////////////////////////////////////////


Material::Material(const Shared<Shader>& shader) :
	_shader(shader)
{
	_shader->Reloaded += SE_BIND_EVENT_FN(Material::OnShaderReloaded);

	AllocateStorage();

	_materialFlags |= static_cast<Uint32>(MaterialFlag::DepthTest);
	_materialFlags |= static_cast<Uint32>(MaterialFlag::Blend);
}

void Material::Bind()
{
	_shader->Bind();

	if (_vSUniformStorageBuffer) _shader->SetVSMaterialUniformBuffer(_vSUniformStorageBuffer);

	if (_pSUniformStorageBuffer) _shader->SetPSMaterialUniformBuffer(_pSUniformStorageBuffer);

	BindTextures();
}

Uint32 Material::GetFlags() const
{
	return _materialFlags;
}

void Material::SetFlag(MaterialFlag flag)
{
	_materialFlags |= static_cast<Uint32>(flag);
}

const Shared<Shader>& Material::GetShader() const
{
	return _shader;
}

void Material::Set(const String& name, const Shared<Texture>& texture)
{
	ShaderResourceDeclaration* decl = FindResourceDeclaration(name);
	const Uint32 slot = decl->GetRegister();
	if (_textures.size() <= slot) _textures.resize(static_cast<size_t>(slot) + 1);
	_textures[slot] = texture;
}

void Material::Set(const String& name, const Shared<Texture2D>& texture)
{
	Set(name, static_cast<const Shared<Texture>&>(texture));
}

void Material::Set(const String& name, const Shared<TextureCube>& texture)
{
	Set(name, static_cast<const Shared<Texture>&>(texture));
}

ShaderResourceDeclaration* Material::FindResourceDeclaration(const String& name)
{
	const auto& resources = _shader->GetResources();
	for (ShaderResourceDeclaration* resource : resources)
	{
		if (resource->GetName() == name) return resource;
	}
	return nullptr;
}

Shared<Material> Material::Create(const Shared<Shader>& shader)
{
	return Shared<Material>::Create(shader);
}

void Material::AllocateStorage()
{
	if (_shader->HasVSMaterialUniformBuffer())
	{
		const auto& vsBuffer = _shader->GetVSMaterialUniformBuffer();
		_vSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
		_vSUniformStorageBuffer.ZeroInitialize();
	}

	if (_shader->HasPSMaterialUniformBuffer())
	{
		const auto& psBuffer = _shader->GetPSMaterialUniformBuffer();
		_pSUniformStorageBuffer.Allocate(psBuffer.GetSize());
		_pSUniformStorageBuffer.ZeroInitialize();
	}
}

bool Material::OnShaderReloaded()
{
	//TODO: WHY IS THIS HERE?!
	return false;
	AllocateStorage();

	for (auto& mi : _materialInstances) mi->OnShaderReloaded();
}

void Material::BindTextures()
{
	Uint32 TextureIndex = 0;
	for (auto& texture : _textures)
	{
		if (texture) texture->Bind(TextureIndex);
		TextureIndex++;
	}
}

ShaderUniformDeclaration* Material::FindUniformDeclaration(const String& name)
{
	if (_vSUniformStorageBuffer)
	{
		const auto& declarations = _shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
		for (ShaderUniformDeclaration* uniform : declarations)
		{
			if (uniform->GetName() == name) return uniform;
		}
	}

	if (_pSUniformStorageBuffer)
	{
		const auto& declarations = _shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
		for (ShaderUniformDeclaration* uniform : declarations)
		{
			if (uniform->GetName() == name) return uniform;
		}
	}
	return nullptr;
}

Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
{
	switch (uniformDeclaration->GetDomain())
	{
	case ShaderDomain::Vertex: return _vSUniformStorageBuffer;
	case ShaderDomain::Pixel: return _pSUniformStorageBuffer;
	default: Debug::Break("Invalid uniform declaration domain! Material does not support this shader type.");
		return _vSUniformStorageBuffer;
	}
}


////////////////////////////////////////////////////////////////
/// Material Instance
////////////////////////////////////////////////////////////////

MaterialInstance::MaterialInstance(const Shared<Material>& material, String name) :
	_material(material),
	_name(Move(name))
{
	_material->_materialInstances.insert(this);
	AllocateStorage();
}


MaterialInstance::~MaterialInstance()
{
	_material->_materialInstances.erase(this);
}

void MaterialInstance::Bind()
{
	_material->_shader->Bind();

	if (_vSUniformStorageBuffer) _material->_shader->SetVSMaterialUniformBuffer(_vSUniformStorageBuffer);

	if (_pSUniformStorageBuffer) _material->_shader->SetPSMaterialUniformBuffer(_pSUniformStorageBuffer);

	_material->BindTextures();

	Uint32 TextureIndex = 0;
	for (auto& texture : _textures)
	{
		if (texture) texture->Bind(TextureIndex);
		TextureIndex++;
	}
}

const String& MaterialInstance::GetName() const
{
	return _name;
}

const Shared<Shader>& MaterialInstance::GetShader() const
{
	return _material->_shader;
}

Uint32 MaterialInstance::GetFlags() const
{
	return _material->_materialFlags;
}

bool MaterialInstance::GetFlag(MaterialFlag flag) const
{
	return static_cast<Uint32>(flag) & _material->_materialFlags;
}

void MaterialInstance::Set(const String& name, const Shared<Texture>& texture)
{
	const auto* decl = _material->FindResourceDeclaration(name);
	if (!decl)
	{
		Log::CoreWarn("Cannot find material property: ", name);
		return;
	}
	const Uint32 slot = decl->GetRegister();
	if (_textures.size() <= slot) _textures.resize(static_cast<size_t>(slot) + 1);
	_textures[slot] = texture;
}

void MaterialInstance::Set(const String& name, const Shared<Texture2D>& texture)
{
	Set(name, static_cast<const Shared<Texture>&>(texture));
}

void MaterialInstance::Set(const String& name, const Shared<TextureCube>& texture)
{
	Set(name, static_cast<const Shared<Texture>&>(texture));
}

Shared<MaterialInstance> MaterialInstance::Create(const Shared<Material>& material)
{
	return Shared<MaterialInstance>::Create(material);
}

void MaterialInstance::AllocateStorage()
{
	if (_material->_shader->HasVSMaterialUniformBuffer())
	{
		const auto& vsBuffer = _material->_shader->GetVSMaterialUniformBuffer();
		_vSUniformStorageBuffer = Buffer::Copy(_material->_vSUniformStorageBuffer.Data(), vsBuffer.GetSize());
	}

	if (_material->_shader->HasPSMaterialUniformBuffer())
	{
		const auto& psBuffer = _material->_shader->GetPSMaterialUniformBuffer();
		_pSUniformStorageBuffer = Buffer::Copy(_material->_pSUniformStorageBuffer.Data(), psBuffer.GetSize());
	}
}

void MaterialInstance::OnShaderReloaded()
{
	AllocateStorage();
	_overriddenValues.clear();
}

Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
{
	switch (uniformDeclaration->GetDomain())
	{
	case ShaderDomain::Vertex: return _vSUniformStorageBuffer;
	case ShaderDomain::Pixel: return _pSUniformStorageBuffer;
	default: Debug::Break("Invalid uniform declaration domain! Material does not support this shader type.");
		return _vSUniformStorageBuffer;
	}
}

void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration* decl)
{
	if (_overriddenValues.find(decl->GetName()) == _overriddenValues.end())
	{
		auto& buffer = GetUniformBufferTarget(decl);
		auto& materialBuffer = _material->GetUniformBufferTarget(decl);
		buffer.Write(materialBuffer.Data() + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
	}
}

void MaterialInstance::SetFlag(MaterialFlag flag, bool value)
{
	if (value)
	{
		_material->_materialFlags |= static_cast<Uint32>(flag);
	}
	else
	{
		_material->_materialFlags &= ~static_cast<Uint32>(flag);
	}
}
}
