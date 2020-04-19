#include "Box.h"

#include "BindableBase.h"

ve::Box::Box( Graphics &gfx,
			  std::mt19937 &rng,
			  std::uniform_real_distribution<float> &adist,
			  std::uniform_real_distribution<float> &ddist,
			  std::uniform_real_distribution<float> &odist,
			  std::uniform_real_distribution<float> &rdist )
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
	struct Vertex
	{
		struct
		{
			float x, y, z;
		}pos;
	};
	const std::vector<Vertex> vertices =
	{
		{ -1.0f,-1.0f,-1.0f },
		{ 1.0f,-1.0f,-1.0f },
		{ -1.0f,1.0f,-1.0f },
		{ 1.0f,1.0f,-1.0f },
		{ -1.0f,-1.0f,1.0f },
		{ 1.0f,-1.0f,1.0f },
		{ -1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
	};
	AddBind( std::make_unique<VertexBuffer>( gfx, vertices ) );

	auto pVS = std::make_unique<VertexShader>( gfx, L"VertexShader.cso" );
	auto pVSbc = pVS->GetBytecode();
	AddBind( std::move( pVS ) );

	AddBind( std::make_unique<PixelShader>( gfx, L"PixelShader.cso" ) );

	const std::vector<unsigned short> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	AddIndexBuffer( std::make_unique<IndexBuffer>( gfx, indices ) );

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
			{ 1.0f,0.0f,1.0f },
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ 0.0f,0.0f,1.0f },
			{ 1.0f,1.0f,0.0f },
			{ 0.0f,1.0f,1.0f },
		}
	};
	AddBind( std::make_unique<PixelConstantBuffer<ConstantBuffer2>>( gfx, cb2 ) );

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddBind( std::make_unique<InputLayout>( gfx, ied, pVSbc ) );

	AddBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_unique<TransformCBuf>( gfx, *this ) );
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
	return DirectX::XMMatrixRotationRollPitchYaw( m_pitch, m_yaw, m_roll ) *
		DirectX::XMMatrixTranslation( m_r, 0.0f, 0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( m_theta, m_phi, m_chi ) *
		DirectX::XMMatrixTranslation( 0.0f, 0.0f, 20.0f );
}
