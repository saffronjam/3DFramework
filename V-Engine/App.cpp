#include "App.h"

#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"

App::App()
	:
	m_wnd( 800, 600, "V-Engine" )
{
	class Factory
	{
	public:
		Factory( ve::Graphics &gfx )
			:
			gfx( gfx )
		{
		}
		std::unique_ptr<ve::Drawable> operator()()
		{
			switch ( typedist( rng ) )
			{
			case 0:
				return std::make_unique<ve::Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<ve::Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<ve::Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			default:
				assert( false && "bad drawable type in factory" );
				return {};
			}
		}
	private:
		ve::Graphics &gfx;
		std::mt19937 rng{ std::random_device{}( ) };
		std::uniform_real_distribution<float> adist{ 0.0f,ve::PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,ve::PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,ve::PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};


	Factory f( m_wnd.gfx );
	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ), nDrawables, f );

	m_wnd.gfx.SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 40.0f ) );
}

App::~App()
{
}

int App::Run()
{
	while ( true )
	{
		m_wnd.kbd.Update();
		m_wnd.mouse.Update();
		if ( const auto exitCode = ve::Window::ProcessMessages() )
			return *exitCode;
		DoFrame();

	}
}

void App::DoFrame()
{
	ve::Time dt = m_clock.GetDeltatime();

	m_wnd.gfx.ClearBuffer( 0.07f, 0.0f, 0.12f );



	for ( auto &drawable : drawables )
	{
		drawable->Update( dt );
		drawable->Draw( m_wnd.gfx );
	}

	m_wnd.gfx.EndFrame();
}
