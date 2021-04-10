#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/GraphicsResource.h"

namespace Se
{
class Shader
{
public:	
	explicit Shader(Filepath filepath);

	bgfx::ShaderHandle GetNativeHandle() const;

	static Shared<Shader> Create(Filepath filepath);

private:
	Filepath _filepath;
	bgfx::ShaderHandle _nativeShaderHandle;

	static constexpr const char* ShaderFolder = "ShaderBin/";
	static constexpr const char* ShaderExtension = ".bin";
};
}
