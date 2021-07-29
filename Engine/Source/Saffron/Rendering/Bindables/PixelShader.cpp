#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/PixelShader.h"

#include <d3dcompiler.h>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
const std::filesystem::path PixelShader::BasePath = {"Assets/Shaders/"};

PixelShader::PixelShader(std::filesystem::path path) :
	_path(std::move(path))
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<PixelShader>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					auto fullpath = BasePath;
					fullpath += inst->_path;
					fullpath += Extension;
					ComPtr<ID3DBlob> blob;
					auto hr = D3DReadFileToBlob(fullpath.c_str(), &blob);
					ThrowIfBad(hr);

					hr = package.Device.CreatePixelShader(
						blob->GetBufferPointer(),
						blob->GetBufferSize(),
						nullptr,
						&inst->_nativePixelShader
					);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void PixelShader::Bind()
{
	const auto inst = ShareThisAs<PixelShader>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.PSSetShader(inst->_nativePixelShader.Get(), nullptr, 0);
		}
	);
}

auto PixelShader::Identifier(const std::filesystem::path& path) -> std::string
{
	return path.string();
}

auto PixelShader::Create(const std::filesystem::path &path) -> std::shared_ptr<PixelShader>
{
	return BindableStore::Add<PixelShader>(path);
}
}
