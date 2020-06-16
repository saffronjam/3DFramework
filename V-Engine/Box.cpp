#include "Box.h"

#include "BindableBase.h"
#include "Cube.h"

ve::Box::Box( Graphics &gfx,
			  std::mt19937 &rng,
			  std::uniform_real_distribution<float> &adist,
			  std::uniform_real_distribution<float> &ddist,
			  std::uniform_real_distribution<float> &odist,
			  std::uniform_real_distribution<float> &rdist,
			  std::uniform_real_distribution<float> &bdist )
	:
	m_r( rdist( rng ) ),
	m_droll( ddist( rng ) ),
	m_dpitch( ddist( rng ) ),
	m_dyaw( ddist( rng ) ),
	m_dphi( odist( rng ) ),
	m_dtheta( odist( rng ) ),
	m_dchi( odist( rng ) ),
	m_chi( adist( rng ) ),
	m_theta( adist( rng ) ),
	m_phi( adist( rng ) )
{

	if ( !IsInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};
		const auto model = Cube::Make<Vertex>();


		StaticBind( std::make_unique<VertexBuffer>( gfx, model.vertices ) );

		//Creates the vertex shader and extracts its bytecode
		auto pVS = std::make_unique<VertexShader>( gfx, L"ColorIndexVS.cso" );
		auto pVSbc = pVS->GetBytecode();
		StaticBind( std::move( pVS ) );

		StaticBind( std::make_unique<PixelShader>( gfx, L"ColorIndexPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx, model.indices ) );

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
		StaticBind( std::make_unique<PSConstantBuffer<PSConstants>>( gfx, psc ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		StaticBind( std::make_unique<InputLayout>( gfx, ied, pVSbc ) );

		StaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCBuf>( gfx, *this ) );
	DirectX::XMStoreFloat3x3(
		&m_modelTransform,
		DirectX::XMMatrixScaling( 1.0f, 1.0f, bdist( rng ) )
	);
}

void ve::Box::Update( const Time &dt ) noexcept
{
	m_roll += m_droll * (float)dt.asSeconds();
	m_pitch += m_dpitch * (float)dt.asSeconds();
	m_yaw += m_dyaw * (float)dt.asSeconds();
	m_theta += m_dtheta * (float)dt.asSeconds();
	m_phi += m_dphi * (float)dt.asSeconds();
	m_chi += m_dchi * (float)dt.asSeconds();
}

DirectX::XMMATRIX ve::Box::GetTransformXM() const noexcept
{
	return
		DirectX::XMLoadFloat3x3( &m_modelTransform ) *
		DirectX::XMMatrixRotationRollPitchYaw( m_pitch, m_yaw, m_roll ) *
		DirectX::XMMatrixTranslation( m_r, 0.0f, 0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( m_theta, m_phi, m_chi ) *
		DirectX::XMMatrixTranslation( 0.0f, 0.0f, 20.0f );
}
