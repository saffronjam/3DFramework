#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/GraphicsResource.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
class Program : public GraphicsResource
{
public:
	explicit Program(const Shared<Shader>& vertexShader, const Shared<Shader>& fragmentShader,
	                 bool destroyAfterCreation = false);
	explicit Program(const Filepath& vsFilepath, const Filepath& fsFilepath);
	explicit Program(const Filepath& commonName);

	bgfx::ProgramHandle GetNativeHandle() const;

	static Shared<Program> Create(Shared<Shader> vertexShader, Shared<Shader> fragmentShader,
	                              bool destroyAfterCreation = false);
	static Shared<Program> Create(Filepath vsFilepath, Filepath fsFilepath);
	static Shared<Program> Create(Filepath commonName);

protected:
	String _filepath;
	bgfx::ProgramHandle _nativeProgramHandle;

	static constexpr const char *VsPrefix = "vs_";
	static constexpr const char *FsPrefix = "fs_";
};
}
