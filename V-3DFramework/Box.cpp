#include "Box.h"

#include "BindableBase.h"
#include "Cube.h"

Box::Box(Graphics& gfx,
		 std::mt19937& rng,
		 std::uniform_real_distribution<float>& adist,
		 std::uniform_real_distribution<float>& ddist,
		 std::uniform_real_distribution<float>& odist,
		 std::uniform_real_distribution<float>& rdist,
		 std::uniform_real_distribution<float>& bdist)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{

	if ( !IsInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};
		const auto model = Cube::Make<Vertex>();


		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		//Creates the vertex shader and extracts its bytecode
		auto pVS = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
		auto pVSbc = pVS->GetBytecode();
		AddStaticBind(std::move(pVS));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));

		AddStaticBind(std::make_unique<IndexBuffer>(gfx, model.indices));

		struct PSConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const PSConstants psc =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(std::make_unique<PSConstantBuffer<PSConstants>>(gfx, psc));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pVSbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	DirectX::XMStoreFloat3x3(
		&m_modelTransform,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&m_modelTransform) * TestObject::GetTransformXM();
}
