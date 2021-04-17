#include "SaffronPCH.h"

#include "Saffron/Editor/EditorTerminal.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
// TODO: Implement as a GUI interface
static Uint64 s_GuiID = 0;

EditorTerminal::EditorTerminal()
{
	m_Sink = std::make_shared<TerminalSink>();
	Clear();
	SetLevel(Log::Level::Info);
	Log::AddClientSink(m_Sink);
}

void EditorTerminal::Clear()
{
	m_Sink->Clear();
}

void EditorTerminal::OnGuiRender()
{
	OutputStringStream oss;
	oss << "Terminal##" << s_GuiID++;

	if (!ImGui::Begin("Terminal"))
	{
		ImGui::End();
		return;
	}

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
	ImGui::SameLine();
	const bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	const bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	m_Filter.Draw("Filter", -100.0f);

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear) Clear();
	if (copy) ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf = m_Sink->m_TextBuffer.begin();
	const char* buf_end = m_Sink->m_TextBuffer.end();
	if (m_Filter.IsActive())
	{
		for (int line_no = 0; line_no < m_Sink->m_LineOffsets.size(); line_no++)
		{
			const char* line_start = buf + m_Sink->m_LineOffsets[line_no];
			const char* line_end = line_no + 1 < m_Sink->m_LineOffsets.size()
				                       ? buf + m_Sink->m_LineOffsets[line_no + 1] - 1
				                       : buf_end;
			if (m_Filter.PassFilter(line_start, line_end)) ImGui::TextUnformatted(line_start, line_end);
		}
	}
	else
	{
		ImGuiListClipper clipper;
		clipper.Begin(m_Sink->m_LineOffsets.size());
		while (clipper.Step())
		{
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
			{
				const char* line_start = buf + m_Sink->m_LineOffsets[line_no];
				const char* line_end = line_no + 1 < m_Sink->m_LineOffsets.size()
					                       ? buf + m_Sink->m_LineOffsets[line_no + 1] - 1
					                       : buf_end;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();
	}
	ImGui::PopStyleVar();

	if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::End();

	s_GuiID--;
}

void EditorTerminal::SetLevel(Log::Level::LevelEnum level)
{
	m_Sink->SetLevel(level);
}
}
