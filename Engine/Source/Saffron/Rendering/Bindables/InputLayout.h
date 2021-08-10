#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
class Shader;

class InputLayout : public Bindable
{
public:
	InputLayout(VertexLayout vertexLayout, const std::shared_ptr<Shader>& vertexShader);

	void Bind() override;

	static std::string Identifier(VertexLayout vertexLayout, const std::shared_ptr<Shader>& vertexShader)
	{
		return vertexLayout.Code();
	}

	static auto Create(
		VertexLayout vertexLayout,
		const std::shared_ptr<Shader>& vertexShader
	) -> std::shared_ptr<InputLayout>;

private:
	ComPtr<ID3D11InputLayout> _nativeLayout{};
	VertexLayout _vertexLayout;
};
}
