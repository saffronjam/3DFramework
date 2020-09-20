#include "SaffronPCH.h"

#include "Saffron/Renderer/Shader.h"
#include "Saffron/Platform/OpenGL/OpenGLShader.h"

namespace Se
{

std::vector<Ref<Shader>> Shader::m_sAllShaders;

Ref<Shader> Shader::Create(const std::string &filepath)
{
	Ref<Shader> result = nullptr;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = Ref<OpenGLShader>::Create(filepath); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	m_sAllShaders.push_back(result);

	return result;
}

Ref<Shader> Shader::CreateFromString(const std::string &source)
{
	Ref<Shader> result = nullptr;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = OpenGLShader::CreateFromString(source); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	m_sAllShaders.push_back(result);

	return result;
}

void ShaderLibrary::Add(const Ref<Shader> &shader)
{
	const auto &name = shader->GetName();
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = shader;
}

void ShaderLibrary::Load(const std::string &path)
{
	const auto shader = Ref<Shader>(Shader::Create(path));
	const auto &name = shader->GetName();
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = shader;
}

void ShaderLibrary::Load(const std::string &name, const std::string &path)
{
	SE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
	m_Shaders[name] = Ref<Shader>(Shader::Create(path));
}

const Ref<Shader> &ShaderLibrary::Get(const std::string &name) const
{
	SE_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
	return m_Shaders.at(name);
}
}
