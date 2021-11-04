#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneEnvironment.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
std::shared_ptr<SceneEnvironment> SceneEnvironment::Create(const std::filesystem::path& path)
{
	const auto inst = std::make_shared<SceneEnvironment>();
	Renderer::Submit(
		[inst, path](const RendererPackage& package)
		{
			const auto envMapSize = Renderer::Config().EnvironmentMapResolution;

			Renderer::BeginStrategy(RenderStrategy::Immediate);
			
			auto equirectangularTex = Texture::Create(path);
			auto cube = TextureCube::Create(envMapSize, envMapSize, ImageFormat::RGBA32f);
			auto shader = Shader::Create("EquirectangularToCubeMap", true);

			D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format = Utils::ToD3D11Format(equirectangularTex->Format());
			desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = -1;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MipSlice = 0;

			/*ComPtr<ID3D11UnorderedAccessView> uav;
			auto hr = package.Device.CreateUnorderedAccessView(&cube->NativeHandle(), &desc, &uav);
			ThrowIfBad(hr);

			package.Context.CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), nullptr);

			shader->Bind();

			package.Context.Dispatch(envMapSize / 32, envMapSize / 32, 6);*/

			Renderer::EndStrategy();
		}
	);

	return nullptr;
}
}
