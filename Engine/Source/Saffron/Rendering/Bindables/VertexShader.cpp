#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/VertexShader.h"

#include <d3dcompiler.h>
#include <utility>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
const std::filesystem::path VertexShader::BasePath = {"Assets/Shaders/"};

VertexShader::VertexShader(std::filesystem::path path) :
	_path(std::move(path))
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<VertexShader>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					auto fullpath = BasePath;
					fullpath += inst->_path.replace_extension();
					fullpath += Extension;
					D3DReadFileToBlob(fullpath.c_str(), &inst->_byteCode);
					const auto hr = package.Device.CreateVertexShader(
						inst->_byteCode->GetBufferPointer(),
						inst->_byteCode->GetBufferSize(),
						nullptr,
						&inst->_nativeVertexShader
					);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void VertexShader::Bind()
{
	const auto inst = ShareThisAs<VertexShader>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.VSSetShader(inst->_nativeVertexShader.Get(), nullptr, 0);
		}
	);
}

auto VertexShader::ByteCode() -> ID3DBlob&
{
	return *_byteCode.Get();
}

auto VertexShader::ByteCode() const -> const ID3DBlob&
{
	return const_cast<VertexShader&>(*this).ByteCode();
}

auto VertexShader::Identifer(std::filesystem::path path) -> std::string
{
	return path.string();
}

auto VertexShader::Create(std::filesystem::path path) -> std::shared_ptr<VertexShader>
{
	return BindableStore::Add<VertexShader>(std::move(path));
}
}
