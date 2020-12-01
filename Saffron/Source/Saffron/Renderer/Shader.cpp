#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Platform/OpenGL/OpenGLShader.h"

namespace Se
{

SignalAggregate<void> Shader::Signals::OnReload;

void Shader::OnGuiRender()
{
	ImGui::Begin("Shaders");

	ResourceManager::ForEach<Shader>([](auto &shader)
									 {
										 if ( ImGui::TreeNode(shader.GetName().c_str()) )
										 {
											 const String buttonName = "Reload##" + shader.GetName();
											 if ( ImGui::Button(buttonName.c_str()) )
												 shader.Reload();
											 ImGui::TreePop();
										 }
									 }
	);

	ImGui::End();
}

std::shared_ptr<Shader> Shader::Create(const Filepath &filepath)
{
	std::shared_ptr<Shader> result;

	const size_t filepathHash = Misc::HashFilepath(filepath);
	if ( ResourceManager::Exists(filepathHash) )
	{
		return ResourceManager::Get(filepathHash)->GetDynShared<Shader>();
	}

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLShader>(filepath); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

std::shared_ptr<Shader> Shader::Create(const String &source)
{
	std::shared_ptr<Shader> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLShader>(source); break;
	default:						SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	}

	return result;
}
}
