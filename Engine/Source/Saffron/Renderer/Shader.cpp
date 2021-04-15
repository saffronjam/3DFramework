#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/Shader.h"
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
		return ResourceManager::Get(filepathHash)->GetDynShared<Shader>();
	}

	switch (RendererAPI::Current())
	{
	case RendererAPI::Type::None: SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		return nullptr;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLShader>(Move(fullFilepath));
		break;
	default: SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
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
	Shared<Shader> result;

	switch (RendererAPI::Current())
	{
	case RendererAPI::Type::None: SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		return nullptr;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLShader>(source);
		break;
	default: SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		return nullptr;
	}

	return result;
}
}
