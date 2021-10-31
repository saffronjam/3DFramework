#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Shader.h"

#include <d3dcompiler.h>
#include <utility>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ShaderStore.h"

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
					inst->_name = inst->_path.stem().string();

					constexpr auto* engineBasePath = SE_ENGINE_ASSETS_PATH;

					// Paths
					const std::filesystem::path vsFullpath = BinaryPath(inst->_path, VsSuffix);
					const std::filesystem::path psFullpath = BinaryPath(inst->_path, PsSuffix);

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

void Shader::Reload()
{
	const auto inst = ShareThisAs<Shader>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			//// Setup

			// Paths
			const std::filesystem::path vsFullpath = TextualPath(inst->_path, VsSuffix);
			const std::filesystem::path psFullpath = TextualPath(inst->_path, PsSuffix);

			///// Compile
			ComPtr<ID3DBlob> errorBlob;

			constexpr auto* vsProfile = "vs_5_0";
			constexpr uint flags = D3DCOMPILE_ENABLE_STRICTNESS | (ConfDebug ? D3DCOMPILE_DEBUG : 0);

			constexpr auto* psProfile = "ps_5_0";
			ComPtr<ID3DBlob> psBlob;

			// Vertex shader
			auto hr = D3DCompileFromFile(
				vsFullpath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				vsProfile,
				flags,
				0,
				&inst->_vsByteCode,
				&errorBlob
			);

			if (BadHResult(hr))
			{
				const auto* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
				throw SaffronException(std::format("Failed to reload shader. Could not compile: {}", errorMsg));
			}

			hr = package.Device.CreateVertexShader(
				inst->_vsByteCode->GetBufferPointer(),
				inst->_vsByteCode->GetBufferSize(),
				nullptr,
				&inst->_nativeVertexShader
			);
			ThrowIfBad(hr);

			// Pixel shader

			hr = D3DCompileFromFile(
				psFullpath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				psProfile,
				flags,
				0,
				&psBlob,
				&errorBlob
			);

			if (BadHResult(hr))
			{
				const auto* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
				throw SaffronException(std::format("Failed to reload shader. Could not compile: {}", errorMsg));
			}

			hr = package.Device.CreatePixelShader(
				psBlob->GetBufferPointer(),
				psBlob->GetBufferSize(),
				nullptr,
				&inst->_nativePixelShader
			);
			ThrowIfBad(hr);
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

auto Shader::Name() const -> const std::string&
{
	return _name;
}

auto Shader::Identifier(std::filesystem::path path) -> std::string
{
	return path.string();
}

auto Shader::Create(std::filesystem::path path) -> std::shared_ptr<Shader>
{
	std::shared_ptr<Shader> shader;
	const auto added = BindableStore::Add<Shader>(shader, std::move(path));
	if (added)
	{
		ShaderStore::Add(shader);
	}
	return shader;
}

auto Shader::BinaryPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path
{
	std::ostringstream oss;
	oss << RootPath << BinBasePath << relative.string() << suffix << BinExtension;
	return oss.str();
}

auto Shader::TextualPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path
{
	std::ostringstream oss;
	oss << RootPath << BasePath << relative.string() << suffix << TextExtension;
	return oss.str();
}
}
