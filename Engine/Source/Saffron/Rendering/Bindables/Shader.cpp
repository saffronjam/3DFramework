#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Shader.h"

#include <d3dcompiler.h>
#include <utility>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ShaderStore.h"

namespace Se
{
Shader::Shader(const std::filesystem::path& path, bool isCompute) :
	_path(path),
	_isCompute(isCompute)
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<Shader>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					inst->_name = inst->_path.stem().string();

					if (inst->_isCompute)
					{
						// Compute shader
						const std::filesystem::path csFullpath = BinaryPath(inst->_path, CsSuffix);
						auto blob = LoadByteCode(csFullpath);
						inst->_nativeComputeShader = CreateComputeShader(package, *blob.Get());
					}
					else
					{
						// Vertex shader
						const std::filesystem::path vsFullpath = BinaryPath(inst->_path, VsSuffix);
						auto blob = LoadByteCode(vsFullpath);
						inst->_vsByteCode = blob;
						inst->_nativeVertexShader = CreateVertexShader(package, *blob.Get());

						// Pixel shader
						const std::filesystem::path psFullpath = BinaryPath(inst->_path, PsSuffix);
						blob = LoadByteCode(psFullpath);
						inst->_nativePixelShader = CreatePixelShader(package, *blob.Get());
					}
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
			if (inst->_isCompute)
			{
				// Compute shader
				const std::filesystem::path csFullpath = TextualPath(inst->_path, CsSuffix);
				auto blob = CompileShader(csFullpath, CsProfile);
				inst->_nativeComputeShader = CreateComputeShader(package, *blob.Get());
			}
			else
			{
				// Vertex shader
				const std::filesystem::path vsFullpath = TextualPath(inst->_path, VsSuffix);
				auto blob = CompileShader(vsFullpath, VsProfile);
				inst->_vsByteCode = blob;
				inst->_nativeVertexShader = CreateVertexShader(package, *blob.Get());

				// Pixel shader
				const std::filesystem::path psFullpath = TextualPath(inst->_path, PsSuffix);
				blob = CompileShader(psFullpath, PsProfile);
				inst->_nativePixelShader = CreatePixelShader(package, *blob.Get());
			}
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
	Debug::Assert(!_isCompute, "Shader was a compute shader");
	return *_nativeVertexShader.Get();
}

auto Shader::NativePsHandle() const -> const ID3D11PixelShader&
{
	Debug::Assert(!_isCompute, "Shader was a compute shader");
	return *_nativePixelShader.Get();
}

auto Shader::NativeCsHandle() const -> const ID3D11ComputeShader&
{
	Debug::Assert(_isCompute, "Shader was not a compute shader");
	return *_nativeComputeShader.Get();
}

auto Shader::Name() const -> const std::string&
{
	return _name;
}

auto Shader::Identifier(const std::filesystem::path& path, bool isCompute) -> std::string
{
	std::ostringstream oss;
	oss << path.string() << (isCompute ? "::Compute" : "::VertexPixel");
	return oss.str();
}

auto Shader::Create(const std::filesystem::path& path, bool isCompute) -> std::shared_ptr<Shader>
{
	std::shared_ptr<Shader> shader;
	const auto added = BindableStore::Add<Shader>(shader, path, isCompute);
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

auto Shader::LoadByteCode(const std::filesystem::path& fullpath) -> ComPtr<ID3DBlob>
{
	ComPtr<ID3DBlob> blob;
	auto hr = D3DReadFileToBlob(fullpath.c_str(), &blob);
	ThrowIfBad(hr);
	return blob;
}

auto Shader::CompileShader(const std::filesystem::path& fullpath, const char* profile) -> ComPtr<ID3DBlob>
{
	constexpr uint flags = D3DCOMPILE_ENABLE_STRICTNESS | (ConfDebug ? D3DCOMPILE_DEBUG : 0);

	ComPtr<ID3DBlob> errorBlob;
	ComPtr<ID3DBlob> shaderBlob;

	auto hr = D3DCompileFromFile(
		fullpath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ShaderEntryPoint,
		profile,
		flags,
		0,
		&shaderBlob,
		&errorBlob
	);

	if (BadHResult(hr))
	{
		const auto* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
		throw SaffronException(std::format("Failed to compile shader: {}", errorMsg));
	}

	return shaderBlob;
}

auto Shader::CreateVertexShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11VertexShader>
{
	ComPtr<ID3D11VertexShader> result;
	const auto hr = package.Device.CreateVertexShader(blob.GetBufferPointer(), blob.GetBufferSize(), nullptr, &result);
	ThrowIfBad(hr);
	return result;
}

auto Shader::CreatePixelShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11PixelShader>
{
	ComPtr<ID3D11PixelShader> result;
	auto hr = package.Device.CreatePixelShader(blob.GetBufferPointer(), blob.GetBufferSize(), nullptr, &result);
	ThrowIfBad(hr);
	return result;
}

auto Shader::CreateComputeShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11ComputeShader>
{
	ComPtr<ID3D11ComputeShader> result;
	const auto hr = package.Device.CreateComputeShader(blob.GetBufferPointer(), blob.GetBufferSize(), nullptr, &result);
	ThrowIfBad(hr);
	return result;
}
}
