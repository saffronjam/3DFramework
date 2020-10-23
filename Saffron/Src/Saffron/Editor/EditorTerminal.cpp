#include "SaffronPCH.h"

#include "Saffron/Editor/EditorTerminal.h"


namespace Se
{

static EditorTerminal s_Terminal;

void EditorTerminal::Init()
{
	s_Terminal.m_Sink = std::make_shared<TerminalSink>();
	Clear();
	SetLevel(Log::Level::Info);
	Log::AddClientSink(s_Terminal.m_Sink);
}

void EditorTerminal::Clear()
{
	s_Terminal.m_Sink->Clear();
}


void EditorTerminal::OnGuiRender()
{
	auto &sink = s_Terminal.m_Sink;


	if ( !ImGui::Begin("Terminal ") )
	{
		ImGui::End();
		return;
	}

	// Options menu
	if ( ImGui::BeginPopup("Options") )
	{
		ImGui::Checkbox("Auto-scroll", &sink->m_AutoScroll);
		ImGui::EndPopup();
	}

	// Main window
	if ( ImGui::Button("Options") )
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	const bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	const bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	sink->m_Filter.Draw("Filter", -100.0f);

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if ( clear )
		Clear();
	if ( copy )
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char *buf = sink->m_Buf.begin();
	const char *buf_end = sink->m_Buf.end();
	if ( sink->m_Filter.IsActive() )
	{
		for ( int line_no = 0; line_no < sink->m_LineOffsets.Size; line_no++ )
		{
			const char *line_start = buf + sink->m_LineOffsets[line_no];
			const char *line_end = (line_no + 1 < sink->m_LineOffsets.Size) ? (buf + sink->m_LineOffsets[line_no + 1] - 1) : buf_end;
			if ( sink->m_Filter.PassFilter(line_start, line_end) )
				ImGui::TextUnformatted(line_start, line_end);
		}
	}
	else
	{
		ImGuiListClipper clipper;
		clipper.Begin(sink->m_LineOffsets.Size);
		while ( clipper.Step() )
		{
			for ( int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++ )
			{
				const char *line_start = buf + sink->m_LineOffsets[line_no];
				const char *line_end = (line_no + 1 < sink->m_LineOffsets.Size)
					? (buf + sink->m_LineOffsets[line_no + 1] - 1)
					: buf_end;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();
	}
	ImGui::PopStyleVar();

	if ( sink->m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::End();
}

void EditorTerminal::SetLevel(Log::Level::LevelEnum level)
{
	s_Terminal.m_Sink->SetLevel(level);
}
}
