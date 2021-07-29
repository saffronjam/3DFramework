#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Bindables/InputLayout.h"
#include "Saffron/Rendering/Bindables/VertexShader.h"

namespace Se
{
InputLayout::InputLayout(VertexLayout vertexLayout, const std::shared_ptr<VertexShader>& vertexShader)
{
	SetInitializer(
		[this, vertexLayout, vertexShader]
		{
			const auto inst = ShareThisAs<InputLayout>();
			Renderer::Submit(
				[inst, vertexLayout, vertexShader](const RendererPackage& package)
				{
					const auto& descs = vertexLayout.Descs();
					auto& byteCode = vertexShader->ByteCode();

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

void InputLayout::Bind()
{
	const auto inst = ShareThisAs<InputLayout>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.IASetInputLayout(inst->_nativeLayout.Get());
		}
	);
}

auto InputLayout::Create(
	VertexLayout vertexLayout,
	const std::shared_ptr<VertexShader>& vertexShader
) -> std::shared_ptr<InputLayout>
{
	return BindableStore::Add<InputLayout>(vertexLayout, vertexShader);
}
}
