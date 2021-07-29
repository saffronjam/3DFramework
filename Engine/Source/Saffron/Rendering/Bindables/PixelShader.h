#pragma once

#include <d3d11_4.h>
#include <filesystem>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class PixelShader : public Bindable
{
public:
	explicit PixelShader(std::filesystem::path path);

	void Bind() override;

	static auto Identifier(const std::filesystem::path& path) -> std::string;
	static auto Create(const std::filesystem::path& path) -> std::shared_ptr<PixelShader>;

private:
	ComPtr<ID3D11PixelShader> _nativePixelShader;
	std::filesystem::path _path;

	static const std::filesystem::path BasePath;
	static constexpr const char* Extension = ".cso";
};
}
