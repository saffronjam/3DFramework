#include "SaffronPCH.h"

#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Editor/SplashScreenPane.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
SplashScreenPane::SplashScreenPane(const std::shared_ptr<BatchLoader> &batchLoader)
	: m_BatchLoader(batchLoader),
	m_Texture(Factory::Create<Texture2D>(Filepath("Resources/Assets/Editor/Saffron.png"))),
	m_FinalizingStatus("Finalizing")
{
}

void SplashScreenPane::OnGuiRender()
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

	const auto logoWidth = 200;
	const auto logoHeight = 200;
	ImGui::SetCursorPos({ windowSize.x / 2.0f - logoWidth / 2.0f, 2.0f * windowSize.y / 5.0f - logoHeight / 2.0f });
	ImGui::Image(reinterpret_cast<ImTextureID>(m_Texture->GetRendererID()), ImVec2(logoWidth, logoHeight), ImVec2(0, 0), ImVec2(1, 1));


	Gui::SetFontSize(36);
	ImGui::NewLine();
	Gui::SetFontSize(48);

	const char *title = "Saffron Engine";
	const float titleTextWidth = ImGui::CalcTextSize(title).x;
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - titleTextWidth / 2.0f);
	ImGui::Text(title);

	/*IM_COL32(106, 58, 206, 255),
	IM_COL32(106, 58, 206, 255),
	IM_COL32(176, 121, 220, 255),
	IM_COL32(176, 121, 220, 255));*/
	Gui::SetFontSize(24);

	oss.str("");
	oss.clear();

	oss << std::setprecision(0) << std::fixed << m_CurrentProgressView << "%";

	ImGui::NewLine();
	const float currentProgressTextWidth = ImGui::CalcTextSize(oss.str().c_str()).x;
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - currentProgressTextWidth / 2.0f);
	ImGui::Text("%s", oss.str().c_str());

	Gui::SetFontSize(18);

	const String *status = m_GoalProgressView < 100.0f ? m_BatchLoader->GetStatus() : &m_FinalizingStatus;
	if ( status && !status->empty() )
	{
		const auto infoTextWidth = ImGui::CalcTextSize(status->c_str()).x;
		ImGui::NewLine();
		ImGui::SetCursorPosX(windowSize.x / 2.0f - infoTextWidth / 2.0f);
		ImGui::Text("%s", status->c_str());
	}

	ImGui::End();
}

void SplashScreenPane::Show()
{
	m_Hidden = false;
}

void SplashScreenPane::Hide()
{
	m_Hidden = true;
}

bool SplashScreenPane::IsIdle() const
{
	return  static_cast<int>(std::round(m_CurrentProgressView)) == static_cast<int>(std::round(m_GoalProgressView));
}

bool SplashScreenPane::IsFinished() const
{
	return m_CurrentProgressView >= 100.0f;
}
}
