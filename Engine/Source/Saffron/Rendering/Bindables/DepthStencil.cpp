#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/DepthStencil.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
DepthStencil::DepthStencil()
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<DepthStencil>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					// Depth test parameters
					inst->_descriptor.DepthEnable = true;
					inst->_descriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					inst->_descriptor.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

					// Stencil test parameters
					inst->_descriptor.StencilEnable = false;
					inst->_descriptor.StencilReadMask = 0xFF;
					inst->_descriptor.StencilWriteMask = 0xFF;

					// Stencil operations if pixel is front-facing
					inst->_descriptor.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					inst->_descriptor.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					inst->_descriptor.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					inst->_descriptor.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Stencil operations if pixel is back-facing
					inst->_descriptor.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					inst->_descriptor.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					inst->_descriptor.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					inst->_descriptor.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					const auto hr = Renderer::Device().CreateDepthStencilState(
						&inst->_descriptor,
						&inst->_nativeDepthStencilState
					);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void DepthStencil::Bind() const
{
	const auto inst = ShareThisAs<const DepthStencil>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_dirty)
			{
				const_cast<DepthStencil&>(*inst).Recreate();
				inst->_dirty = false;
			}
			package.Context.OMSetDepthStencilState(inst->_nativeDepthStencilState.Get(), 0);
		}
	);
}

void DepthStencil::SetEnabled(bool enabled)
{
	const auto inst = ShareThisAs<DepthStencil>();
	Renderer::Submit(
		[inst, enabled](const RendererPackage& package)
		{
			inst->_descriptor.DepthEnable = enabled;
			inst->_dirty = true;
		}
	);
}

auto DepthStencil::Create() -> std::shared_ptr<DepthStencil>
{
	return BindableStore::Add<DepthStencil>();
}

void DepthStencil::Recreate()
{
	//const auto hr = Renderer::Device().CreateDepthStencilState(&_descriptor, &_nativeDepthStencilState);
	//ThrowIfBad(hr);
}
}
