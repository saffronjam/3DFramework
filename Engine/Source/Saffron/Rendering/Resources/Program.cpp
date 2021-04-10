#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Program.h"

namespace Se
{
Program::Program(const Shared<Shader>& vertexShader, const Shared<Shader>& fragmentShader, bool destroyAfterCreation)
{
	_nativeProgramHandle = bgfx::createProgram(vertexShader->GetNativeHandle(), fragmentShader->GetNativeHandle(),
	                                           destroyAfterCreation);
}

Program::Program(const Filepath& vsFilepath, const Filepath& fsFilepath)
{
	const auto vs = Shader::Create(VsPrefix + vsFilepath.string());
	const auto fs = Shader::Create(FsPrefix + fsFilepath.string());
	_nativeProgramHandle = bgfx::createProgram(vs->GetNativeHandle(), fs->GetNativeHandle(), true);
}

Program::Program(const Filepath& commonName) :
	Program(commonName, commonName)
{
}

bgfx::ProgramHandle Program::GetNativeHandle() const
{
	return _nativeProgramHandle;
}

Shared<Program> Program::Create(Shared<Shader> vertexShader, Shared<Shader> fragmentShader, bool destroyAfterCreation)
{
	return CreateShared<Program>(Move(vertexShader), Move(fragmentShader), destroyAfterCreation);
}

Shared<Program> Program::Create(Filepath vsFilepath, Filepath fsFilepath)
{
	return CreateShared<Program>(Move(vsFilepath), Move(fsFilepath));
}

Shared<Program> Program::Create(Filepath commonName)
{
	return CreateShared<Program>(Move(commonName));
}
}
