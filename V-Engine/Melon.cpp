#include "Melon.h"

#include "BindableBase.h"
#include "Sphere.h"

ve::Melon::Melon( Graphics &gfx, std::mt19937 &rng, std::uniform_real_distribution<float> &adist, std::uniform_real_distribution<float> &ddist, std::uniform_real_distribution<float> &odist, std::uniform_real_distribution<float> &rdist, std::uniform_int_distribution<int> &longdist, std::uniform_int_distribution<int> &latdist )
	:
	r( rdist( rng ) ),
	droll( ddist( rng ) ),
	dpitch( ddist( rng ) ),
	dyaw( ddist( rng ) ),
	dphi( odist( rng ) ),
	dtheta( odist( rng ) ),
	dchi( odist( rng ) ),
	chi( adist( rng ) ),
	theta( adist( rng ) ),
	phi( adist( rng ) )
{
	if ( !IsInitialized() )
	{
		auto pvs = std::make_unique<VertexShader>( gfx, L"ColorIndexVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		StaticBind( std::move( pvs ) );

		StaticBind( std::make_unique<PixelShader>( gfx, L"ColorIndexPS.cso" ) );

		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const PixelShaderConstants cb2 =
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
		StaticBind( std::make_unique<PSConstantBuffer<PixelShaderConstants>>( gfx, cb2 ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		StaticBind( std::make_unique<InputLayout>( gfx, ied, pvsbc ) );

		StaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
	};
	auto model = Sphere::Make<Vertex>( latdist( rng ), longdist( rng ) );
	// deform vertices of model by linear transformation
	model.Transform( DirectX::XMMatrixScaling( 1.0f, 1.0f, 1.2f ) );

	AddBind( std::make_unique<VertexBuffer>( gfx, model.vertices ) );

	AddIndexBuffer( std::make_unique<IndexBuffer>( gfx, model.indices ) );

	AddBind( std::make_unique<TransformCBuf>( gfx, *this ) );
}

void ve::Melon::Update( const Time &dt ) noexcept
{
	roll += droll * dt.asSeconds();
	pitch += dpitch * dt.asSeconds();
	yaw += dyaw * dt.asSeconds();
	theta += dtheta * dt.asSeconds();
	phi += dphi * dt.asSeconds();
	chi += dchi * dt.asSeconds();
}

DirectX::XMMATRIX ve::Melon::GetTransformXM() const noexcept
{
	return
		DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, roll ) *
		DirectX::XMMatrixTranslation( r, 0.0f, 0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta, phi, chi ) *
		DirectX::XMMatrixTranslation( 0.0f, 0.0f, 20.0f );
}
