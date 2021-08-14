#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Shader.h"

#include <d3dcompiler.h>
#include <utility>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Shader::Shader(std::filesystem::path path) :
	_path(std::move(path))
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<Shader>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					// Paths
					std::ostringstream oss;
					oss << BasePath << inst->_path.string() << VsSuffix << Extension;
					const std::filesystem::path vsFullpath = oss.str();

					oss.str("");
					oss.clear();

					oss << BasePath << inst->_path.string() << PsSuffix << Extension;
					const std::filesystem::path psFullpath = oss.str();

					// Vertex shader
					auto hr = D3DReadFileToBlob(vsFullpath.c_str(), &inst->_vsByteCode);
					ThrowIfBad(hr);

					hr = package.Device.CreateVertexShader(
						inst->_vsByteCode->GetBufferPointer(),
						inst->_vsByteCode->GetBufferSize(),
						nullptr,
						&inst->_nativeVertexShader
					);
					ThrowIfBad(hr);

					// Pixel shader
					ComPtr<ID3DBlob> blob;
					hr = D3DReadFileToBlob(psFullpath.c_str(), &blob);
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

void Shader::Bind() const
{
	const auto inst = ShareThisAs<const Shader>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.VSSetShader(inst->_nativeVertexShader.Get(), nullptr, 0);
			package.Context.PSSetShader(inst->_nativePixelShader.Get(), nullptr, 0);
		}
	);
}

void Shader::Unbind() const
{
	const auto inst = ShareThisAs<const Shader>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.VSSetShader(nullptr, nullptr, 0);
			package.Context.PSSetShader(nullptr, nullptr, 0);
		}
	);
}

auto Shader::VsByteCode() -> ID3DBlob&
{
	if (_vsByteCode == nullptr)
	{
		throw SaffronException(std::format("No byte code in vertex shader: {}", _path));
	}
	return *_vsByteCode.Get();
}

auto Shader::VsByteCode() const -> const ID3DBlob&
{
	return const_cast<Shader&>(*this).VsByteCode();
}

auto Shader::NativeVsHandle() const -> const ID3D11VertexShader&
{
	return *_nativeVertexShader.Get();
}

auto Shader::NativePsHandle() const -> const ID3D11PixelShader&
{
	return *_nativePixelShader.Get();
}

auto Shader::Identifer(std::filesystem::path path) -> std::string
{
	return path.string();
}

auto Shader::Create(std::filesystem::path path) -> std::shared_ptr<Shader>
{
	return BindableStore::Add<Shader>(std::move(path));
}
}
