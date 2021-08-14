#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Bindables/InputLayout.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{
InputLayout::InputLayout(VertexLayout vertexLayout, const std::shared_ptr<Shader>& shader) :
	_vertexLayout(vertexLayout)
{
	SetInitializer(
		[this, shader]
		{
			const auto inst = ShareThisAs<InputLayout>();
			Renderer::Submit(
				[inst, shader](const RendererPackage& package)
				{
					const auto& descs = inst->_vertexLayout.Descs();
					auto& byteCode = shader->VsByteCode();

					const auto hr = package.Device.CreateInputLayout(
						descs.data(),
						descs.size(),
						byteCode.GetBufferPointer(),
						byteCode.GetBufferSize(),
						&inst->_nativeLayout
					);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void InputLayout::Bind() const
{
	const auto inst = ShareThisAs<const InputLayout>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.IASetInputLayout(inst->_nativeLayout.Get());
		}
	);
}

auto InputLayout::Create(
	VertexLayout vertexLayout,
	const std::shared_ptr<Shader>& vertexShader
) -> std::shared_ptr<InputLayout>
{
	return BindableStore::Add<InputLayout>(vertexLayout, vertexShader);
}
}
