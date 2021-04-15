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

Shared<Shader> Shader::Create(Filepath filepath)
{
	Shared<Shader> result;

	Filepath fullFilepath = ShaderLocation + Move(filepath).string() + ShaderExtension;

	const size_t filepathHash = Misc::HashFilepath(fullFilepath);
	if (ResourceManager::Exists(filepathHash))
	{
		return ResourceManager::Get(filepathHash);
	}

	switch (RendererAPI::Current())
	{
	case RendererApiType::None: SE_CORE_FALSE_ASSERT("RendererApi::None is currently not supported!");
		return nullptr;
	case RendererApiType::OpenGL: result = Shared<OpenGLShader>::Create(Move(fullFilepath));
		break;
	default: SE_CORE_FALSE_ASSERT("RendererApi::None is currently not supported!");
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
	switch (RendererAPI::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return OpenGLShader::Create(source);
	}
	SE_CORE_FALSE_ASSERT("RendererApi::None is currently not supported!");
	return nullptr;
}
}
