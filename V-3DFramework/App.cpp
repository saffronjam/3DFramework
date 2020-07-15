#include "App.h"

#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "SkinnedBox.h"

namespace dx = DirectX;

App::App()
	:
	m_wnd(800, 600, "V-Engine"),
	m_mainLight(m_wnd.gfx),
	m_speedFactor(1.0f)
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{
		}

		const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };

		std::unique_ptr<Drawable> operator()()
		{
			switch ( sdist(rng) )
			{
			case 0:
				return std::make_unique<Model>(
					gfx, rng, adist, ddist,
					odist, rdist, mat, 1.5f
					);
			case 1:
				return std::make_unique<Model>(
					gfx, rng, adist, ddist,
					odist, rdist, mat, 1.5f
					);
			case 2:
				return std::make_unique<Model>(
					gfx, rng, adist, ddist,
					odist, rdist, mat, 1.5f
					);
			case 3:
				return std::make_unique<Model>(
					gfx, rng, adist, ddist,
					odist, rdist, mat, 1.5f
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,3 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
	};


	Factory f(m_wnd.gfx);
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	m_wnd.gfx.SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
		if ( const auto exitCode = Window::ProcessMessages() )
			return *exitCode;
		DoFrame();

	}
}

void App::DoFrame()
{
	Time dt = m_clock.GetDeltatime();

	m_wnd.gfx.ClearBuffer(0.07f, 0.0f, 0.12f);
	m_wnd.gfx.BeginFrame();

	m_wnd.gfx.SetCamera(m_camera.GetMatrix());
	m_mainLight.Bind(m_wnd.gfx, m_camera.GetMatrix());


	for ( auto& drawable : drawables )
	{
		drawable->Update(dt);
		//drawable->Draw(m_wnd.gfx);
	}

	SpawnSimulationWindow();
	m_camera.SpawnControlWindow();
	m_mainLight.SpawnControlWindow();

	m_wnd.gfx.EndFrame();
}

void App::SpawnSimulationWindow() noexcept
{
	if ( ImGui::Begin("Simulation Speed") )
	{
		ImGui::SliderFloat("Speed Factor", &m_speedFactor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", m_wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();
}