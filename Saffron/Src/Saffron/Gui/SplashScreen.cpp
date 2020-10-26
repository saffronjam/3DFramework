#include "SaffronPCH.h"

#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/SplashScreen.h"

namespace Se
{
SplashScreen::SplashScreen()
	: m_BatchLoader(BatchLoader::GetPreloader()),
	m_Texture(Texture2D::Create(Filepath("Assets/Editor/Saffron.png"))) //TODO: Add proper texture
{
}

void SplashScreen::OnGuiRender()
{
	if ( m_Hidden )
		return;

	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	OutputStringStream oss;
	oss << "Loading Screen##";
	ImGui::Begin(oss.str().c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	if ( m_BatchLoader->GetProgress() != m_GoalProgressView )
	{
		m_GoalProgressView = m_BatchLoader->GetProgress();
		m_CurrentSinTimer = 0.0f;
	}
	else
	{
		m_CurrentSinTimer += GlobalTimer::GetStep().sec();
	}
	m_CurrentProgressView += (m_GoalProgressView - m_CurrentProgressView) * std::sin(m_CurrentSinTimer / (2.0f * Math::PI));

	const auto windowSize = ImGui::GetWindowSize();

	const auto logoWidth = 332;
	const auto logoHeight = 250;
	ImGui::SetCursorPos({ windowSize.x / 2.0f - logoWidth / 2.0f, windowSize.y / 2.0f - logoHeight / 2.0f });
	ImGui::Image(reinterpret_cast<ImTextureID>(m_Texture->GetRendererID()), ImVec2(logoWidth, logoHeight), ImVec2(0, 0), ImVec2(1, 1));

	const auto loadingBarFullWidth = windowSize.x / 1.5f;
	const auto loadingBarWidth = windowSize.x / 1.5f / 100.0f * m_CurrentProgressView;
	const auto loadingBarHeight = windowSize.y / 20.0f;
	ImGui::SetCursorPos({ windowSize.x / 2.0f - loadingBarFullWidth / 2.0f, windowSize.y / 2.0f + logoHeight / 2.0f + 20.0f });
	const ImVec2 loadingBarFullPosition = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled({ loadingBarFullPosition.x, loadingBarFullPosition.y }, { loadingBarFullPosition.x + loadingBarFullWidth, loadingBarFullPosition.y + loadingBarHeight }, ImGui::GetColorU32(ImGuiCol_TextDisabled), 6.0f);

	ImGui::SetCursorPos({ windowSize.x / 2.0f - loadingBarWidth / 2.0f, windowSize.y / 2.0f + logoHeight / 2.0f + 20.0f });
	const ImVec2 loadingBarPosition = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled({ loadingBarPosition.x, loadingBarPosition.y }, { loadingBarPosition.x + loadingBarWidth, loadingBarPosition.y + loadingBarHeight }, IM_COL32(97, 51, 113, 255), 6.0f);

	const String *status = m_BatchLoader->GetStatus();
	if ( status && !status->empty() )
	{
		const auto infoTextWidth = ImGui::CalcTextSize(status->c_str()).x;
		ImGui::SetCursorPos({ windowSize.x / 2.0f - infoTextWidth / 2.0f, loadingBarPosition.y + loadingBarHeight * 1.2f });
		ImGui::Text("%s", status->c_str());
	}

	ImGui::End();
}

void SplashScreen::Show()
{
	m_Hidden = false;
}

void SplashScreen::Hide()
{
	m_Hidden = true;
}
}
