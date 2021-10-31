#pragma once

#include <d3d11_4.h>
#include <filesystem>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class Shader : public Bindable
{
public:
	explicit Shader(std::filesystem::path path);

	void Bind() const override;
	void Unbind() const override;

	void Reload();

	auto VsByteCode() -> ID3DBlob&;
	auto VsByteCode() const -> const ID3DBlob&;

	auto NativeVsHandle() const -> const ID3D11VertexShader&;
	auto NativePsHandle() const -> const ID3D11PixelShader&;

	auto Name() const -> const std::string&;

	static auto Identifier(std::filesystem::path path) -> std::string;
	static auto Create(std::filesystem::path path) -> std::shared_ptr<Shader>;

private:
	static auto BinaryPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path;
	static auto TextualPath(const std::filesystem::path& relative, const char* suffix) -> std::filesystem::path;

private:
	ComPtr<ID3D11VertexShader> _nativeVertexShader{};
	ComPtr<ID3D11PixelShader> _nativePixelShader;
	ComPtr<ID3DBlob> _vsByteCode{};

	std::filesystem::path _path;
	std::string _name;

	static constexpr const char* RootPath = SE_ENGINE_ASSETS_PATH;
	static constexpr const char* BasePath = "Shaders/";
	static constexpr const char* BinBasePath = "Shaders/Bin/";
	static constexpr const char* TextExtension = ".hlsl";
	static constexpr const char* BinExtension = ".cso";
	static constexpr const char* VsSuffix = "_v";
	static constexpr const char* PsSuffix = "_p";
};
}
