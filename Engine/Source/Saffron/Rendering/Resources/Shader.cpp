#include "SaffronPCH.h"

#include <bx/readerwriter.h>
#include <bx/file.h>

#include "Saffron/Common/FileIOManager.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
Shader::Shader(Filepath filepath) :
	_filepath(ShaderFolder + Move(filepath).string() + ShaderExtension)
{
	bx::FileReader reader;

	const bx::FilePath bxFp(_filepath.string().c_str());
	const bool didOpen = bx::open(&reader, bxFp);
	SE_CORE_ASSERT(didOpen);

	const auto size = static_cast<uint32_t>(bx::getSize(&reader));
	const bgfx::Memory* mem = bgfx::alloc(size + 1);
	bx::read(&reader, mem->data, size);
	bx::close(&reader);
	mem->data[mem->size - 1] = '\0';

	_nativeShaderHandle = bgfx::createShader(mem);
}

bgfx::ShaderHandle Shader::GetNativeHandle() const
{
	return _nativeShaderHandle;
}

Shared<Shader> Shader::Create(Filepath filepath)
{
	return CreateShared<Shader>(Move(filepath));
}
}
