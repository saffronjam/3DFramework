#pragma once

#include <d3d11_4.h>
#include <filesystem>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
struct RendererPackage;

class Shader : public Bindable
{
public:
	explicit Shader(const std::filesystem::path&, bool isCompute);

	void Bind() const override;
	void Unbind() const override;

	void Reload();

	auto VsByteCode() -> ID3DBlob&;
	auto VsByteCode() const -> const ID3DBlob&;

	auto NativeVsHandle() const -> const ID3D11VertexShader&;
	auto NativePsHandle() const -> const ID3D11PixelShader&;
	auto NativeCsHandle() const -> const ID3D11ComputeShader&;

	auto Name() const -> const std::string&;

	static auto Identifier(const std::filesystem::path&, bool isCompute) -> std::string;
	static auto Create(const std::filesystem::path&, bool isCompute = false) -> std::shared_ptr<Shader>;

private:
	static auto BinaryPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path;
	static auto TextualPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path;


	// Helper wrappers for creating 
	static auto LoadByteCode(const std::filesystem::path& fullpath) -> ComPtr<ID3DBlob>;

	static auto CompileShader(const std::filesystem::path& fullpath, const char* profile) -> ComPtr<ID3DBlob>;

	static auto CreateVertexShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11VertexShader>;
	static auto CreatePixelShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11PixelShader>;
	static auto CreateComputeShader(const RendererPackage& package, ID3DBlob& blob) -> ComPtr<ID3D11ComputeShader>;

private:
	ComPtr<ID3D11VertexShader> _nativeVertexShader{};
	ComPtr<ID3D11PixelShader> _nativePixelShader{};
	ComPtr<ID3D11ComputeShader> _nativeComputeShader{};
	ComPtr<ID3DBlob> _vsByteCode{};

	std::filesystem::path _path;
	std::string _name;
	bool _isCompute = false;

	static constexpr const char* RootPath = SE_ENGINE_ASSETS_PATH;
	static constexpr const char* BasePath = "Shaders/";
	static constexpr const char* BinBasePath = "Shaders/Bin/";
	static constexpr const char* TextExtension = ".hlsl";
	static constexpr const char* BinExtension = ".cso";
	static constexpr const char* VsSuffix = "_v";
	static constexpr const char* PsSuffix = "_p";
	static constexpr const char* CsSuffix = "_c";
	static constexpr const char* VsProfile = "vs_5_0";
	static constexpr const char* PsProfile = "ps_5_0";
	static constexpr const char* CsProfile = "cs_5_0";
	static constexpr const char* ShaderEntryPoint = "main";
};
}
