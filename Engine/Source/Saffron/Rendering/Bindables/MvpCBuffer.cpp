#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/MvpCBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
MvpCBuffer::MvpCBuffer() :
	MvpCBuffer({Matrix::Identity, Matrix::Identity, Matrix::Identity})
{
}

MvpCBuffer::MvpCBuffer(const std::array<Matrix, 3>& mvp) :
	_mvp(mvp)
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<MvpCBuffer>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					// Since Hlsl in column major
					std::array<Matrix, 3> matrices;
					for (int i = 0; i < matrices.size(); i++)
					{
						matrices[i] = inst->_mvp[i].Transpose();
					}

					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DYNAMIC;
					bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					bd.MiscFlags = 0;
					bd.ByteWidth = sizeof(matrices);
					bd.StructureByteStride = 0;

					D3D11_SUBRESOURCE_DATA sd = {};
					sd.pSysMem = matrices.data();

					package.Device.CreateBuffer(&bd, &sd, &inst->_nativeBuffer);
				}
			);
		}
	);
}

void MvpCBuffer::Bind()
{
	const auto inst = ShareThisAs<MvpCBuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			if (inst->_dirty)
			{
				// Since Hlsl in column major
				std::array<Matrix, 3> matrices;
				for (int i = 0; i < matrices.size(); i++)
				{
					matrices[i] = inst->_mvp[i].Transpose();
				}

				D3D11_MAPPED_SUBRESOURCE mr = {};
				const auto hr = package.Context.Map(inst->_nativeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
				ThrowIfBad(hr);

				std::memcpy(mr.pData, matrices.data(), sizeof(matrices));

				package.Context.Unmap(inst->_nativeBuffer.Get(), 0);
				inst->_dirty = false;
			}
			package.Context.VSSetConstantBuffers(0, 1, inst->_nativeBuffer.GetAddressOf());
		}
	);
}

void MvpCBuffer::UpdateMatrix(const std::array<Matrix, 3>& mvp)
{
	const auto inst = ShareThisAs<MvpCBuffer>();
	Renderer::Submit(
		[inst, mvp](const RendererPackage& package)
		{
			inst->_mvp = mvp;
			inst->_dirty = true;
		}
	);
}

auto MvpCBuffer::Create() -> std::shared_ptr<MvpCBuffer>
{
	return BindableStore::Add<MvpCBuffer>();
}

auto MvpCBuffer::Create(const std::array<Matrix, 3>& mvp) -> std::shared_ptr<MvpCBuffer>
{
	return BindableStore::Add<MvpCBuffer>(mvp);
}
}
