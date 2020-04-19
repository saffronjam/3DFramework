#include "App.h"

App::App()
	:
	m_wnd( 800, 600, "V-Engine" )
{
	std::mt19937 rng( std::random_device{}( ) );
	std::uniform_real_distribution<float> adist( 0.0f, 3.1415f * 2.0f );
	std::uniform_real_distribution<float> ddist( 0.0f, 3.1415f * 2.0f );
	std::uniform_real_distribution<float> odist( 0.0f, 3.1415f * 0.3f );
	std::uniform_real_distribution<float> rdist( 6.0f, 20.0f );
	for ( int i = 0; i < 80; i++ )
	{
		boxes.push_back( std::make_unique<ve::Box>(
			m_wnd.gfx,
			rng,
			adist,
			ddist,
			odist,
			rdist
			)
		);
	}
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



	for ( auto &box : boxes )
	{
		box->Update( dt );
		box->Draw( m_wnd.gfx );
	}

	m_wnd.gfx.EndFrame();
}
