#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Platform/OpenGL/OpenGLShader.h"

namespace Se
{
void Shader::OnGuiRender()
{
	ImGui::Begin("Shaders");

	ResourceManager::ForEach<Shader>([](auto& shader)
	{
		if (ImGui::TreeNode(shader.GetName().c_str()))
		{
			const String buttonName = "Reload##" + shader.GetName();
			if (ImGui::Button(buttonName.c_str())) shader.Reload();
			ImGui::TreePop();
		}
	});

	ImGui::End();
}

size_t Shader::GetResourceID()
{
	return _filepath.empty() ? 0ull : Misc::HashFilepath(_filepath);
}

Shared<Shader> Shader::Create(Filepath filepath)
{
	Shared<Shader> result;

	Filepath fullFilepath = ShaderLocation + Move(filepath).string() + ShaderExtension;

	const size_t filepathHash = Misc::HashFilepath(fullFilepath);
	if (ResourceManager::Exists(filepathHash))
	{
		return ResourceManager::Get(filepathHash);
	}

	switch (RendererApi::Current())
	{
	case RendererApiType::None: Debug::Break("RendererApi::None is currently not supported!");
		return nullptr;
	case RendererApiType::OpenGL: result = Shared<OpenGLShader>::Create(Move(fullFilepath));
		break;
	default: Debug::Break("RendererApi::None is currently not supported!");
		return nullptr;
	}

	if (result)
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

Shared<Shader> Shader::Create(const Buffer& source)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return OpenGLShader::Create(source);
	}
	Debug::Break("RendererApi::None is currently not supported!");
	return nullptr;
}
}
