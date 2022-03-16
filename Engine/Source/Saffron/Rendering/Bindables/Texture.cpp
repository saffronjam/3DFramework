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
					td.BindFlags = Utils::ToD3D11TextureBindFlags(inst->_spec.Usage);
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

					auto& texRef = *inst->_nativeTexture.Get();
					auto meta = DirectX::TexMetadata{
						.width = td.Width, .height = td.Height, .mipLevels = td.MipLevels, .format = td.Format
					};
					if (inst->_spec.Usage & TextureUsage_ShaderResource)
					{
						inst->_nativeShaderResourceView = CreateSrv(package, texRef, meta);
					}
					if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
					{
						inst->_nativeUnorderedAccessView = CreateUav(package, texRef, meta);
					}
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

					//// Create texture
					// TODO: Support DDS, TGA...
					bool isHdr = inst->_path->extension() == ".hdr";

					auto [tex, meta] = isHdr
						                   ? LoadHdrFromFile(package, inst->_path.value(), inst->_spec)
						                   : LoadWicFromFile(package, inst->_path.value(), inst->_spec);
					inst->_nativeTexture = tex;

					//// Create views
					auto& texRef = *inst->_nativeTexture.Get();
					if (inst->_spec.Usage & TextureUsage_ShaderResource)
					{
						inst->_nativeShaderResourceView = CreateSrv(package, texRef, meta);
					}
					if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
					{
						inst->_nativeUnorderedAccessView = CreateUav(package, texRef, meta);
					}

					//// Convert metadata
					inst->_width = meta.width;
					inst->_height = meta.height;
					inst->_format = Utils::ToSaffronFormat(meta.format);
					inst->_loaded = true;
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

					if (usage & ImageUsage_ShaderResource && inst->_spec.Usage & TextureUsage_ShaderResource)
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
			if (inst->_spec.Usage & TextureUsage_ShaderResource)
			{
				const auto srv = inst->_nativeShaderResourceView.GetAddressOf();
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_PS)
				{
					package.Context.PSSetShaderResources(slot, 1, srv);
				}
				if (inst->_bindFlags & BindFlag_VS)
				{
					package.Context.VSSetShaderResources(slot, 1, srv);
				}
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetShaderResources(slot, 1, srv);
				}
			}
			if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
			{
				const auto srv = inst->_nativeUnorderedAccessView.GetAddressOf();
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetUnorderedAccessViews(slot, 1, srv, nullptr);
				}
				if constexpr (ConfDebug)
				{
					if (inst->_bindFlags & BindFlag_VS || inst->_bindFlags & BindFlag_PS)
					{
						Log::Warn("Trying to bind uav to non-compute shader");
					}
				}
			}

			if (inst->_spec.CreateSampler)
			{
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				inst->_sampler->Bind();
				Renderer::EndStrategy();
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
			if (inst->_spec.Usage & TextureUsage_ShaderResource)
			{
				const auto slot = inst->_slot;
				ID3D11ShaderResourceView* srv = nullptr;
				if (inst->_bindFlags & BindFlag_PS)
				{
					package.Context.PSSetShaderResources(slot, 1, &srv);
				}
				if (inst->_bindFlags & BindFlag_VS)
				{
					package.Context.VSSetShaderResources(slot, 1, &srv);
				}
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetShaderResources(slot, 1, &srv);
				}
			}

			if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
			{
				ID3D11UnorderedAccessView* uav = nullptr;
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
				}
				if constexpr (ConfDebug)
				{
					if (inst->_bindFlags & BindFlag_VS || inst->_bindFlags & BindFlag_PS)
					{
						Log::Warn("Trying to unbind uav from non-compute shader");
					}
				}
			}


			if (inst->_spec.CreateSampler)
			{
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				inst->_sampler->Unbind();
				Renderer::EndStrategy();
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

auto Texture::BindFlags() const -> ShaderBindFlags
{
	return _bindFlags;
}

void Texture::SetImage(const std::shared_ptr<Image>& image)
{
	const auto imageInst = image;
	const auto inst = ShareThisAs<Texture>();
	Renderer::Submit(
		[inst, imageInst](const RendererPackage& package)
		{
			const auto usage = imageInst->Usage();

			if (usage & ImageUsage_ShaderResource && inst->_spec.Usage & TextureUsage_ShaderResource)
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

void Texture::SetBindFlags(ShaderBindFlags flags)
{
	const auto inst = ShareThisAs<Texture>();
	Renderer::Submit(
		[inst, flags](const RendererPackage& package)
		{
			inst->_bindFlags = flags;
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

auto Texture::UnorderedView() -> ID3D11UnorderedAccessView&
{
	return *_nativeUnorderedAccessView.Get();
}

auto Texture::UnorderedView() const -> const ID3D11UnorderedAccessView&
{
	return *_nativeUnorderedAccessView.Get();
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

auto Texture::CreateSrv(
	const RendererPackage& package,
	ID3D11Texture2D& tex,
	const DirectX::TexMetadata& meta
) -> ComPtr<ID3D11ShaderResourceView>
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = meta.format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = meta.mipLevels;
	srvd.Texture2D.MostDetailedMip = 0;

	ComPtr<ID3D11ShaderResourceView> result;
	const auto hr = package.Device.CreateShaderResourceView(&tex, &srvd, &result);
	ThrowIfBad(hr);
	return result;
}

auto Texture::CreateUav(
	const RendererPackage& package,
	ID3D11Texture2D& tex,
	const DirectX::TexMetadata& meta
) -> ComPtr<ID3D11UnorderedAccessView>
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	uavd.Format = meta.format;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavd.Texture2D.MipSlice = meta.mipLevels; // Unsure...

	ComPtr<ID3D11UnorderedAccessView> result;
	const auto hr = package.Device.CreateUnorderedAccessView(&tex, &uavd, &result);
	ThrowIfBad(hr);
	return result;
}

auto Texture::LoadWicFromFile(
	const RendererPackage& package,
	const std::filesystem::path& path,
	const TextureSpec& spec
) -> std::tuple<ComPtr<ID3D11Texture2D>, DirectX::TexMetadata>
{
	const auto wicFlags = spec.SRGB ? DirectX::WIC_FLAGS_FORCE_SRGB : DirectX::WIC_FLAGS_FORCE_RGB;

	// Load from disk
	DirectX::TexMetadata metaData{};
	DirectX::GetMetadataFromWICFile(path.c_str(), wicFlags, metaData);

	DirectX::ScratchImage scratchImage;
	DirectX::LoadFromWICFile(path.c_str(), wicFlags, &metaData, scratchImage);

	auto res = CreateFromScratchAndMeta(package, scratchImage, metaData, spec);

	// TODO: Store this for some time in case it is loaded from disk again
	scratchImage.Release();

	return std::make_tuple(res, metaData);
}

auto Texture::LoadHdrFromFile(
	const RendererPackage& package,
	const std::filesystem::path& path,
	const TextureSpec& spec
) -> std::tuple<ComPtr<ID3D11Texture2D>, DirectX::TexMetadata>
{
	// Load from disk
	DirectX::TexMetadata metaData{};
	DirectX::GetMetadataFromHDRFile(path.c_str(), metaData);

	DirectX::ScratchImage scratchImage;
	DirectX::LoadFromHDRFile(path.c_str(), &metaData, scratchImage);

	auto res = CreateFromScratchAndMeta(package, scratchImage, metaData, spec);

	// TODO: Store this for some time in case it is loaded from disk again
	scratchImage.Release();

	return std::make_tuple(res, metaData);
}

auto Texture::CreateFromScratchAndMeta(
	const RendererPackage& package,
	const DirectX::ScratchImage& scratch,
	const DirectX::TexMetadata& meta,
	const TextureSpec& spec
) -> ComPtr<ID3D11Texture2D>
{
	// Convert to texture
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = meta.width;
	td.Height = meta.height;
	td.Format = meta.format;
	td.ArraySize = 1;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = Utils::ToD3D11TextureBindFlags(spec.Usage);
	td.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = scratch.GetPixels();
	// Works as long as we expect exactly one image in each file
	sd.SysMemPitch = scratch.GetImages()->rowPitch;

	ComPtr<ID3D11Texture2D> texResult;
	auto hr = package.Device.CreateTexture2D(&td, &sd, &texResult);
	ThrowIfBad(hr);

	return texResult;
}

TextureCube::TextureCube(uint width, uint height, ImageFormat format, const TextureSpec& spec, uint slot) :
	_width(width),
	_height(height),
	_format(format),
	_spec(spec),
	_slot(slot)
{
	SetInitializer(
		[=]
		{
			const auto inst = ShareThisAs<TextureCube>();
			Renderer::Submit(
				[=](const RendererPackage& package)
				{
					//// Create texture
					D3D11_TEXTURE2D_DESC td = {};
					td.Width = width;
					td.Height = height;
					td.MipLevels = 1;
					td.ArraySize = 6; // For each cube face
					td.Format = Utils::ToDxgiTextureFormat(format);
					td.SampleDesc.Count = 1;
					td.SampleDesc.Quality = 0;
					td.Usage = D3D11_USAGE_DEFAULT;
					td.BindFlags = Utils::ToD3D11TextureBindFlags(inst->_spec.Usage);
					td.CPUAccessFlags = 0;
					td.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

					Color* arr = new Color[1024 * 1024];

					for (int i = 0; i < 1024 * 1024; i++)
					{
						arr[i] = Colors::Red;
					}

					D3D11_SUBRESOURCE_DATA data[6];
					for (int i = 0; i < 6; i++)
					{
						data[i].pSysMem = arr;
						data[i].SysMemPitch = 1024;
						data[i].SysMemSlicePitch = 0;
					}

					auto hr = package.Device.CreateTexture2D(&td, data, &inst->_nativeTexture);
					ThrowIfBad(hr);

					//// Create views
					auto& texRef = *inst->_nativeTexture.Get();
					auto meta = DirectX::TexMetadata{
						.width = td.Width, .height = td.Height, .mipLevels = td.MipLevels, .format = td.Format
					};
					if (inst->_spec.Usage & TextureUsage_ShaderResource)
					{
						inst->_nativeShaderResourceView = CreateSrv(package, texRef, meta);
					}
					if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
					{
						inst->_nativeUnorderedAccessView = CreateUav(package, texRef, meta);
					}
				}
			);
			if (_spec.CreateSampler)
			{
				_sampler = Sampler::Create({_spec.SamplerWrap, _spec.SamplerFilter, {0.0f, 0.0f, 0.0f, 1.0f}}, _slot);
			}
		}
	);
}

void TextureCube::Bind() const
{
	const auto inst = ShareThisAs<const TextureCube>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_spec.Usage & TextureUsage_ShaderResource)
			{
				const auto srv = inst->_nativeShaderResourceView.GetAddressOf();
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_PS)
				{
					package.Context.PSSetShaderResources(slot, 1, srv);
				}
				if (inst->_bindFlags & BindFlag_VS)
				{
					package.Context.VSSetShaderResources(slot, 1, srv);
				}
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetShaderResources(slot, 1, srv);
				}
			}
			if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
			{
				const auto srv = inst->_nativeUnorderedAccessView.GetAddressOf();
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetUnorderedAccessViews(slot, 1, srv, nullptr);
				}
				if constexpr (ConfDebug)
				{
					if (inst->_bindFlags & BindFlag_VS || inst->_bindFlags & BindFlag_PS)
					{
						Log::Warn("Trying to bind uav to non-compute shader");
					}
				}
			}

			if (inst->_spec.CreateSampler)
			{
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				inst->_sampler->Bind();
				Renderer::EndStrategy();
			}
		}
	);
}

void TextureCube::Unbind() const
{
	const auto inst = ShareThisAs<const TextureCube>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_spec.Usage & TextureUsage_ShaderResource)
			{
				const auto slot = inst->_slot;
				ID3D11ShaderResourceView* srv = nullptr;
				if (inst->_bindFlags & BindFlag_PS)
				{
					package.Context.PSSetShaderResources(slot, 1, &srv);
				}
				if (inst->_bindFlags & BindFlag_VS)
				{
					package.Context.VSSetShaderResources(slot, 1, &srv);
				}
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetShaderResources(slot, 1, &srv);
				}
			}

			if (inst->_spec.Usage & TextureUsage_UnorderedAccess)
			{
				ID3D11UnorderedAccessView* uav = nullptr;
				const auto slot = inst->_slot;
				if (inst->_bindFlags & BindFlag_CS)
				{
					package.Context.CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
				}
				if constexpr (ConfDebug)
				{
					if (inst->_bindFlags & BindFlag_VS || inst->_bindFlags & BindFlag_PS)
					{
						Log::Warn("Trying to unbind uav from non-compute shader");
					}
				}
			}

			if (inst->_spec.CreateSampler)
			{
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				inst->_sampler->Unbind();
				Renderer::EndStrategy();
			}
		}
	);
}

auto TextureCube::NativeHandle() -> ID3D11Texture2D&
{
	return *_nativeTexture.Get();
}

auto TextureCube::NativeHandle() const -> const ID3D11Texture2D&
{
	return *_nativeTexture.Get();
}

auto TextureCube::ShaderView() -> ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto TextureCube::ShaderView() const -> const ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto TextureCube::UnorderedView() -> ID3D11UnorderedAccessView&
{
	return *_nativeUnorderedAccessView.Get();
}

auto TextureCube::UnorderedView() const -> const ID3D11UnorderedAccessView&
{
	return *_nativeUnorderedAccessView.Get();
}

auto TextureCube::Width() const -> uint
{
	return _width;
}

auto TextureCube::Height() const -> uint
{
	return _height;
}

auto TextureCube::Format() const -> ImageFormat
{
	return _format;
}

auto TextureCube::BindFlags() const -> ShaderBindFlags
{
	return _bindFlags;
}

auto TextureCube::Usage() const -> TextureUsage
{
	return _spec.Usage;
}

void TextureCube::SetBindFlags(ShaderBindFlags flags)
{
	const auto inst = ShareThisAs<TextureCube>();
	Renderer::Submit(
		[inst, flags](const RendererPackage& package)
		{
			inst->_bindFlags = flags;
		}
	);
}

void TextureCube::SetUsage(TextureUsage usage)
{
	const auto inst = ShareThisAs<TextureCube>();
	Renderer::Submit(
		[inst, usage](const RendererPackage& package)
		{
			inst->_spec.Usage = usage;
			return;

			//// Code below actually cleans up views, which isnt actually needed since they are so light weight...
			D3D11_TEXTURE2D_DESC td;
			inst->_nativeTexture->GetDesc(&td);
			auto meta = DirectX::TexMetadata{
				.width = td.Width, .height = td.Height, .mipLevels = td.MipLevels, .format = td.Format
			};
			auto& texRef = *inst->_nativeTexture.Get();
			auto& oldUsage = inst->_spec.Usage;

			const auto srvDiff = (oldUsage & TextureUsage_ShaderResource) != (usage & TextureUsage_ShaderResource);
			if (srvDiff)
			{
				if (oldUsage & TextureUsage_ShaderResource)
				{
					inst->_nativeShaderResourceView.ReleaseAndGetAddressOf();
				}
				else
				{
					inst->_nativeShaderResourceView = CreateSrv(package, texRef, meta);
				}
			}
			const auto uavDiff = (oldUsage & TextureUsage_UnorderedAccess) != (usage & TextureUsage_UnorderedAccess);
			if (uavDiff)
			{
				if (oldUsage & TextureUsage_UnorderedAccess)
				{
					inst->_nativeUnorderedAccessView.ReleaseAndGetAddressOf();
				}
				else
				{
					inst->_nativeUnorderedAccessView = CreateUav(package, texRef, meta);
				}
			}
			oldUsage = usage;
		}
	);
}

auto TextureCube::Create(
	uint width,
	uint height,
	ImageFormat format,
	const TextureSpec& spec,
	uint slot
) -> std::shared_ptr<TextureCube>
{
	return BindableStore::Add<TextureCube>(width, height, format, spec, slot);
}

auto TextureCube::CreateSrv(
	const RendererPackage& package,
	ID3D11Texture2D& tex,
	const DirectX::TexMetadata& meta
) -> ComPtr<ID3D11ShaderResourceView>
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = meta.format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.Texture2D.MipLevels = meta.mipLevels;
	srvd.Texture2D.MostDetailedMip = 0;

	ComPtr<ID3D11ShaderResourceView> result;
	const auto hr = package.Device.CreateShaderResourceView(&tex, &srvd, &result);
	ThrowIfBad(hr);
	return result;
}

auto TextureCube::CreateUav(
	const RendererPackage& package,
	ID3D11Texture2D& tex,
	const DirectX::TexMetadata& meta
) -> ComPtr<ID3D11UnorderedAccessView>
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
	uavd.Format = meta.format;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavd.Texture2DArray.ArraySize = -1;
	uavd.Texture2DArray.FirstArraySlice = 0;
	uavd.Texture2DArray.MipSlice = 0;

	ComPtr<ID3D11UnorderedAccessView> result;
	auto hr = package.Device.CreateUnorderedAccessView(&tex, &uavd, &result);
	ThrowIfBad(hr);
	return result;
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
