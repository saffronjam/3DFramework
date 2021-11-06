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

template <class T>
class ConstantBuffer : public Bindable
{
public:
	explicit ConstantBuffer(uint slot);
	explicit ConstantBuffer(const T& initialData, uint slot);

	void Bind() const override;
	void Unbind() const override;

	void Update(const T& data);
	virtual void UploadData();

	void SetBindFlags(ConstantBufferBindFlags bindFlags);

	auto Data() -> T&;
	auto Data() const -> const T&;

	static auto Create(uint slot = 0) -> std::shared_ptr<ConstantBuffer>;
	static auto Create(const T& initialData, uint slot = 0) -> std::shared_ptr<ConstantBuffer>;

protected:
	T _data;

private:
	ComPtr<ID3D11Buffer> _nativeBuffer;

	uint _slot;
	bool _dirty = false;
	ConstantBufferBindFlags _bindFlags = ConstantBufferBindFlags_VS;
};

template <class T>
ConstantBuffer<T>::ConstantBuffer(uint slot) :
	ConstantBuffer({}, slot)
{
}

template <class T>
ConstantBuffer<T>::ConstantBuffer(const T& initialData, uint slot) :
	_data(initialData),
	_slot(slot)
{
	SetInitializer(
		[this]
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

					const auto hr = package.Device.CreateBuffer(&bd, &sd, &inst->_nativeBuffer);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

template <class T>
void ConstantBuffer<T>::Bind() const
{
	const auto inst = ShareThisAs<const ConstantBuffer<T>>();

	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_dirty)
			{
				const_cast<ConstantBuffer<T>&>(*inst).UploadData();
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

template <class T>
void ConstantBuffer<T>::Unbind() const
{
	const auto inst = ShareThisAs<const ConstantBuffer<T>>();

	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_bindFlags & ConstantBufferBindFlags_VS)
			{
				constexpr ID3D11Buffer* buffer = nullptr;
				package.Context.VSSetConstantBuffers(inst->_slot, 1, &buffer);
			}
			if (inst->_bindFlags & ConstantBufferBindFlags_PS)
			{
				constexpr ID3D11Buffer* buffer = nullptr;
				package.Context.PSSetConstantBuffers(inst->_slot, 1, &buffer);
			}
		}
	);
}

template <class T>
void ConstantBuffer<T>::Update(const T& data)
{
	const auto inst = ShareThisAs<ConstantBuffer<T>>();
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			inst->_data = data;
			inst->_dirty = true;
		}
	);
}

template <class T>
void ConstantBuffer<T>::UploadData()
{
	const auto inst = ShareThisAs<ConstantBuffer<T>>();
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

template <class T>
void ConstantBuffer<T>::SetBindFlags(ConstantBufferBindFlags bindFlags)
{
	const auto inst = ShareThisAs<ConstantBuffer>();
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			inst->_bindFlags = bindFlags;
		}
	);
}

template <class T>
auto ConstantBuffer<T>::Data() -> T&
{
	return _data;
}

template <class T>
auto ConstantBuffer<T>::Data() const -> const T&
{
	return const_cast<ConstantBuffer<T>&>(*this).Data();
}

template <class T>
auto ConstantBuffer<T>::Create(uint slot) -> std::shared_ptr<ConstantBuffer>
{
	return BindableStore::Add<ConstantBuffer>(slot);
}

template <class T>
auto ConstantBuffer<T>::Create(const T& initialData, uint slot) -> std::shared_ptr<ConstantBuffer>
{
	return BindableStore::Add<ConstantBuffer>(initialData, slot);
}
}
