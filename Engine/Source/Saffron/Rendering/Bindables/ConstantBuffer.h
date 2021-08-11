#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
using ConstantBufferBindFlags = uint;

enum ConstantBufferBindFlags_
{
	ConstantBufferBindFlags_VS = 1 << 0,
	ConstantBufferBindFlags_PS = 1 << 1
};

template <class T, bool Deferred = true>
class ConstantBuffer : public Bindable
{
public:
	explicit ConstantBuffer(const T& initialData, uint slot);

	void Bind() const override;

	void Update(const T& data);
	virtual void UploadData();

	void SetBindFlags(ConstantBufferBindFlags bindFlags);

	auto Data() -> T&;
	auto Data() const -> const T&;

	static auto Create(const T& initialData, uint slot = 0) -> std::shared_ptr<ConstantBuffer>;

protected:
	T _data;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer;

	uint _slot;
	bool _dirty = false;
	ConstantBufferBindFlags _bindFlags = ConstantBufferBindFlags_VS;
};

template <class T, bool Deferred>
ConstantBuffer<T, Deferred>::ConstantBuffer(const T& initialData, uint slot) :
	_slot(slot),
	_data(initialData)
{
	SetInitializer(
		[this]
		{
			if constexpr (Deferred)
			{
				const auto inst = ShareThisAs<ConstantBuffer>();
				Renderer::Submit(
					[inst](const RendererPackage& package)
					{
						D3D11_BUFFER_DESC bd;
						bd.Usage = D3D11_USAGE_DYNAMIC;
						bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bd.MiscFlags = 0;
						bd.ByteWidth = sizeof T;
						bd.StructureByteStride = 0;

						D3D11_SUBRESOURCE_DATA sd = {};
						sd.pSysMem = &inst->_data;

						const auto hr = Renderer::Device().CreateBuffer(&bd, &sd, &inst->_nativeBuffer);
						ThrowIfBad(hr);
					}
				);
			}
			else
			{
				D3D11_BUFFER_DESC bd;
				bd.Usage = D3D11_USAGE_DYNAMIC;
				bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bd.MiscFlags = 0;
				bd.ByteWidth = sizeof T;
				bd.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA sd = {};
				sd.pSysMem = &_data;

				const auto hr = Renderer::Device().CreateBuffer(&bd, &sd, &_nativeBuffer);
				ThrowIfBad(hr);
			}
		}
	);
}

template <class T, bool Deferred>
void ConstantBuffer<T, Deferred>::Bind() const
{
	if constexpr (Deferred)
	{
		const auto inst = ShareThisAs<const ConstantBuffer<T, Deferred>>();
		Renderer::Submit(
			[inst](const RendererPackage& package)
			{
				if (inst->_dirty)
				{
					const_cast<ConstantBuffer<T, Deferred>&>(*inst).UploadData();
				}

				if (inst->_bindFlags & ConstantBufferBindFlags_VS)
				{
					package.Context.VSSetConstantBuffers(inst->_slot, 1, inst->_nativeBuffer.GetAddressOf());
				}
				if (inst->_bindFlags & ConstantBufferBindFlags_PS)
				{
					package.Context.PSSetConstantBuffers(inst->_slot, 1, inst->_nativeBuffer.GetAddressOf());
				}
			}
		);
	}
	else
	{
		if (_dirty)
		{
			const_cast<ConstantBuffer<T, Deferred>&>(*this).UploadData();
		}

		auto& context = Renderer::Context();
		if (_bindFlags & ConstantBufferBindFlags_VS)
		{
			context.VSSetConstantBuffers(_slot, 1, _nativeBuffer.GetAddressOf());
		}
		if (_bindFlags & ConstantBufferBindFlags_PS)
		{
			context.PSSetConstantBuffers(_slot, 1, _nativeBuffer.GetAddressOf());
		}
	}
}

template <class T, bool Deferred>
void ConstantBuffer<T, Deferred>::Update(const T& data)
{
	if constexpr (Deferred)
	{
		const auto inst = ShareThisAs<ConstantBuffer>();
		Renderer::Submit(
			[=](const RendererPackage& package)
			{
				inst->_data = data;
				inst->_dirty = true;
			}
		);
	}
	else
	{
		_data = data;
		_dirty = true;
	}
}

template <class T, bool Deferred>
void ConstantBuffer<T, Deferred>::UploadData()
{
	if constexpr (Deferred)
	{
		const auto inst = ShareThisAs<ConstantBuffer<T, Deferred>>();
		Renderer::Submit(
			[inst](const RendererPackage& package)
			{
				D3D11_MAPPED_SUBRESOURCE mr = {};
				const auto hr = package.Context.Map(inst->_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
				ThrowIfBad(hr);

				std::memcpy(mr.pData, &inst->_data, sizeof T);

				package.Context.Unmap(inst->_nativeBuffer.Get(), 0);

				inst->_dirty = false;
			}
		);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE mr = {};
		const auto hr = Renderer::Context().Map(_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
		ThrowIfBad(hr);

		std::memcpy(mr.pData, &_data, sizeof T);

		Renderer::Context().Unmap(_nativeBuffer.Get(), 0);

		_dirty = false;
	}
}

template <class T, bool Deferred>
void ConstantBuffer<T, Deferred>::SetBindFlags(ConstantBufferBindFlags bindFlags)
{
	if constexpr (Deferred)
	{
		const auto inst = ShareThisAs<ConstantBuffer>();
		Renderer::Submit(
			[=](const RendererPackage& package)
			{
				inst->_bindFlags = bindFlags;
			}
		);
	}
	else
	{
		_bindFlags = bindFlags;
	}
}

template <class T, bool Deferred>
auto ConstantBuffer<T, Deferred>::Data() -> T&
{
	return _data;
}

template <class T, bool Deferred>
auto ConstantBuffer<T, Deferred>::Data() const -> const T&
{
	return const_cast<ConstantBuffer<T>&>(*this).Data();
}

template <class T, bool Deferred>
auto ConstantBuffer<T, Deferred>::Create(const T& initialData, uint slot) -> std::shared_ptr<ConstantBuffer>
{
	return BindableStore::Add<ConstantBuffer>(initialData, slot);
}
}
