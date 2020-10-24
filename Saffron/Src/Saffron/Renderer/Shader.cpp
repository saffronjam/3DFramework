#include "SaffronPCH.h"

#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Platform/OpenGL/OpenGLShader.h"

namespace Se
{

ArrayList<Shared<Shader>> Shader::m_sAllShaders;

void Shader::OnGuiRender()
{
	ImGui::Begin("Shaders");

	auto &shaders = Shader::m_sAllShaders;
	for ( auto &shader : shaders )
	{
		if ( ImGui::TreeNode(shader->GetName().c_str()) )
		{
			String buttonName = "Reload##" + shader->GetName();
			if ( ImGui::Button(buttonName.c_str()) )
				shader->Reload();
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

Shared<Shader> Shader::Create(const String &filepath)
{
	Shared<Shader> result = nullptr;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = Shared<OpenGLShader>::Create(filepath); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	m_sAllShaders.push_back(result);

	return result;
}

Shared<Shader> Shader::CreateFromString(const String &source)
{
	Shared<Shader> result = nullptr;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = OpenGLShader::CreateFromString(source); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	m_sAllShaders.push_back(result);

	return result;
}

void ShaderLibrary::Add(const Shared<Shader> &shader)
{
	const auto &name = shader->GetName();
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = shader;
}

void ShaderLibrary::Load(const String &path)
{
	const auto shader = Shared<Shader>(Shader::Create(path));
	const auto &name = shader->GetName();
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = shader;
}

void ShaderLibrary::Load(const String &name, const String &path)
{
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = Shared<Shader>(Shader::Create(path));
}

const Shared<Shader> &ShaderLibrary::Get(const String &name) const
{
	SE_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
	return m_Shaders.at(name);
}
}
