#pragma once

#include <d3d11_4.h>
#include <filesystem>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class VertexShader : public Bindable
{
public:
	explicit VertexShader(std::filesystem::path path);

	void Bind() override;

	auto ByteCode() -> ID3DBlob&;
	auto ByteCode() const -> const ID3DBlob&;

	static auto Identifer(std::filesystem::path path) -> std::string;
	static auto Create(std::filesystem::path path) -> std::shared_ptr<VertexShader>;

private:
	ComPtr<ID3D11VertexShader> _nativeVertexShader{};
	ComPtr<ID3DBlob> _byteCode{};

	std::filesystem::path _path;

	static const std::filesystem::path BasePath;
	static constexpr const wchar_t* Extension = L".cso";
};
}
