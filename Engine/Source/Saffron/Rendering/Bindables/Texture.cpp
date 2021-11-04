#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Texture.h"

#include <DirectXTK/WICTextureLoader.h>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

#include "DirectXTex/DirectXTex.h"

namespace Se
{
Texture::Texture(const TextureSpec& spec, uint slot) :
	_spec(spec),
	_slot(slot)
{
	SetInitializer(
		[this]
		{
			if (_spec.CreateSampler)
			{
				_sampler = Sampler::Create({_spec.SamplerWrap, _spec.SamplerFilter, {0.0f, 0.0f, 0.0f, 1.0f}}, _slot);
			}
		}
	);
}

Texture::Texture(uint width, uint height, ImageFormat format, const uchar* data, const TextureSpec& spec, uint slot) :
	_spec(spec),
	_format(format),
	_width(width),
	_height(height),
	_slot(slot)
{
	if (data != nullptr)
	{
		_dataBuffer = {data, data + width * height * Utils::ImageFormatToMemorySize(format)};
	}

	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<Texture>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					D3D11_TEXTURE2D_DESC td = {};
					td.Width = inst->_width;
					td.Height = inst->_height;
					td.Format = Utils::ToDxgiTextureFormat(inst->_format);
					td.ArraySize = 1;
					td.MipLevels = 1;
					td.MiscFlags = 0;
					td.SampleDesc.Count = 1;
					td.Usage = D3D11_USAGE_DEFAULT;
					td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
					td.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA sd = {};
					const bool hasData = !inst->_dataBuffer.empty();
					if (hasData)
					{
						sd.pSysMem = inst->_dataBuffer.data();
						sd.SysMemPitch = inst->_width * Utils::ImageFormatToMemorySize(inst->_format);
					}

					auto hr = package.Device.CreateTexture2D(&td, hasData ? &sd : nullptr, &inst->_nativeTexture);
					ThrowIfBad(hr);

					D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
					srvd.Format = td.Format;
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvd.Texture2D.MipLevels = td.MipLevels;
					srvd.Texture2D.MostDetailedMip = 0;

					hr = package.Device.CreateShaderResourceView(
						inst->_nativeTexture.Get(),
						&srvd,
						&inst->_nativeShaderResourceView
					);
					ThrowIfBad(hr);
				}
			);

			if (_spec.CreateSampler)
			{
				_sampler = Sampler::Create({_spec.SamplerWrap, _spec.SamplerFilter, {0.0f, 0.0f, 0.0f, 1.0f}}, _slot);
			}
		}
	);
}

Texture::Texture(const std::filesystem::path& path, const TextureSpec& spec, uint slot) :
	_spec(spec),
	_slot(slot),
	_path(std::make_optional(path))
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<Texture>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					if (!std::filesystem::is_regular_file(inst->_path.value()))
					{
						throw SaffronException(
							std::format("Failed to create texture. Path was not a file: {}", inst->_path->string())
						);
					}

					if (!std::filesystem::exists(inst->_path.value()))
					{
						throw SaffronException(
							std::format("Failed to create texture. File does not exist: {}", inst->_path->string())
						);
					}

					const auto* path = inst->_path->c_str();
					const auto loaderFlags = inst->_spec.SRGB
						                         ? DirectX::WIC_LOADER_FORCE_RGBA32
						                         : DirectX::WIC_LOADER_FORCE_SRGB;


					bool isHdr = inst->_path->extension() == ".hdr";

					if (isHdr)
					{
						// Load from disk
						DirectX::TexMetadata metaData;
						DirectX::GetMetadataFromHDRFile(path, metaData);

						DirectX::ScratchImage scratchImage;
						DirectX::LoadFromHDRFile(path, &metaData, scratchImage);

						inst->_width = metaData.width;
						inst->_height = metaData.height;
						inst->_format = Utils::ToSaffronFormat(metaData.format);

						// Convert to texture
						D3D11_TEXTURE2D_DESC td = {};
						td.Width = inst->_width;
						td.Height = inst->_height;
						td.Format = metaData.format;
						td.ArraySize = 1;
						td.MipLevels = 1;
						td.MiscFlags = 0;
						td.SampleDesc.Count = 1;
						td.Usage = D3D11_USAGE_DEFAULT;
						td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
						td.CPUAccessFlags = 0;

						D3D11_SUBRESOURCE_DATA sd = {};
						sd.pSysMem = scratchImage.GetPixels();
						sd.SysMemPitch = inst->_width * Utils::ImageFormatToMemorySize(inst->_format);

						auto hr = package.Device.CreateTexture2D(&td, &sd, &inst->_nativeTexture);
						ThrowIfBad(hr);

						D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
						srvd.Format = td.Format;
						srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						srvd.Texture2D.MipLevels = td.MipLevels;
						srvd.Texture2D.MostDetailedMip = 0;

						hr = package.Device.CreateShaderResourceView(
							inst->_nativeTexture.Get(),
							&srvd,
							&inst->_nativeShaderResourceView
						);
						ThrowIfBad(hr);

						// TODO: Store this for some time in case it is loaded from disk again
						scratchImage.Release();
					}
					else
					{
						// Load from disk
						ComPtr<ID3D11Resource> texture;
						const auto hr = DirectX::CreateWICTextureFromFileEx(
							&package.Device,
							path,
							0,
							D3D11_USAGE_DEFAULT,
							D3D11_BIND_SHADER_RESOURCE,
							0,
							0,
							loaderFlags,
							&texture,
							&inst->_nativeShaderResourceView
						);
						ThrowIfBad(hr);
						texture->QueryInterface(__uuidof(ID3D11Texture2D), &inst->_nativeTexture);
					}

					inst->_loaded = true;

					// Fetch texture info
					D3D11_TEXTURE2D_DESC td;
					inst->_nativeTexture->GetDesc(&td);

					inst->_format = Utils::ToSaffronFormat(td.Format);
					inst->_width = td.Width;
					inst->_height = td.Height;
				}
			);

			if (_spec.CreateSampler)
			{
				_sampler = Sampler::Create({_spec.SamplerWrap, _spec.SamplerFilter, {1.0f, 1.0f, 1.0f, 1.0f}}, _slot);
			}
		}
	);
}

Texture::Texture(const std::shared_ptr<Image>& image, const TextureSpec& spec, uint slot) :
	_spec(spec),
	_slot(slot)
{
	auto imageInst = image;
	SetInitializer(
		[this, imageInst]
		{
			const auto inst = ShareThisAs<Texture>();
			Renderer::Submit(
				[inst, imageInst](const RendererPackage& package)
				{
					const auto usage = imageInst->Usage();

					if (usage & ImageUsage_ShaderResource)
					{
						inst->_nativeShaderResourceView = imageInst->_nativeShaderResourceView;
					}
					inst->_nativeTexture = imageInst->_nativeTexture;

					inst->_width = imageInst->Width();
					inst->_height = imageInst->Height();
					inst->_format = imageInst->Format();
				}
			);

			if (_spec.CreateSampler)
			{
				_sampler = Sampler::Create({_spec.SamplerWrap, _spec.SamplerFilter, {1.0f, 1.0f, 1.0f, 1.0f}}, _slot);
			}
		}
	);
}

void Texture::Bind() const
{
	const auto inst = ShareThisAs<const Texture>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.PSSetShaderResources(inst->_slot, 1, inst->_nativeShaderResourceView.GetAddressOf());
			if (inst->_spec.CreateSampler)
			{
				inst->_sampler->Bind();
			}
		}
	);
}

void Texture::Unbind() const
{
	const auto inst = ShareThisAs<const Texture>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			ID3D11ShaderResourceView* srv = nullptr;
			package.Context.PSSetShaderResources(inst->_slot, 1, &srv);
			if (inst->_spec.CreateSampler)
			{
				inst->_sampler->Unbind();
			}
		}
	);
}

auto Texture::Width() const -> uint
{
	return _width;
}

auto Texture::Height() const -> uint
{
	return _height;
}

auto Texture::Format() const -> ImageFormat
{
	return _format;
}

auto Texture::Loaded() const -> bool
{
	return _loaded;
}

void Texture::SetImage(const std::shared_ptr<Image>& image)
{
	const auto imageInst = image;
	const auto inst = ShareThisAs<Texture>();
	Renderer::Submit(
		[inst, imageInst](const RendererPackage& package)
		{
			const auto usage = imageInst->Usage();

			if (usage & ImageUsage_ShaderResource)
			{
				inst->_nativeShaderResourceView = imageInst->_nativeShaderResourceView;
			}
			inst->_nativeTexture = imageInst->_nativeTexture;

			inst->_width = imageInst->Width();
			inst->_height = imageInst->Height();
			inst->_format = imageInst->Format();
		}
	);
}

auto Texture::NativeHandle() -> ID3D11Texture2D&
{
	return *_nativeTexture.Get();
}

auto Texture::NativeHandle() const -> const ID3D11Texture2D&
{
	return *_nativeTexture.Get();
}

auto Texture::ShaderView() -> ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto Texture::ShaderView() const -> const ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto Texture::Create(const TextureSpec& spec, uint slot) -> std::shared_ptr<Texture>
{
	return BindableStore::Add<Texture>(spec, slot);
}

auto Texture::Create(
	uint width,
	uint height,
	ImageFormat format,
	const uchar* data,
	const TextureSpec& spec,
	uint slot
) -> std::shared_ptr<Texture>
{
	return BindableStore::Add<Texture>(width, height, format, data, spec, slot);
}

auto Texture::Create(const std::filesystem::path& path, const TextureSpec& spec, uint slot) -> std::shared_ptr<Texture>
{
	return BindableStore::Add<Texture>(path, spec, slot);
}

auto Texture::Create(
	const std::shared_ptr<Image>& image,
	const TextureSpec& spec,
	uint slot
) -> std::shared_ptr<Texture>
{
	return BindableStore::Add<Texture>(image, spec, slot);
}

size_t Utils::ImageFormatToMemorySize(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGBA: return 4;
	case ImageFormat::RGBA32f: return 4 * sizeof(float);
	default: break;
	}

	throw SaffronException("Invalid format. Could not convert to memory size.");
}
}
