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

	auto VsByteCode() -> ID3DBlob&;
	auto VsByteCode() const -> const ID3DBlob&;

	auto NativeVsHandle() const -> const ID3D11VertexShader&;
	auto NativePsHandle() const -> const ID3D11PixelShader&;

	static auto Identifer(std::filesystem::path path) -> std::string;
	static auto Create(std::filesystem::path path) -> std::shared_ptr<Shader>;

private:
	ComPtr<ID3D11VertexShader> _nativeVertexShader{};
	ComPtr<ID3D11PixelShader> _nativePixelShader;
	ComPtr<ID3DBlob> _vsByteCode{};

	std::filesystem::path _path;

	static constexpr const char* BasePath = "Assets/Shaders/";
	static constexpr const char* Extension = ".cso";
	static constexpr const char* VsSuffix = "_v";
	static constexpr const char* PsSuffix = "_p";
};
}
