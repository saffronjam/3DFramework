#pragma once

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");

#include <mutex>

#include "Saffron/Core/Log.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
class GuiTerminal : public spdlog::sinks::base_sink<std::mutex>
{
public:
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.
	std::array<std::unique_ptr<spdlog::formatter>, 3> terminal_formatter_{}; // user_input, error, cmd_history_completion (c.f. ImTerm::message::type)

	GuiTerminal()
	{
		set_level(spdlog::level::info);
		AutoScroll = true;
		Clear();
	}

	void Clear()
	{
		Buf.clear();
		Buf.append("");
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}


	void Draw(const char *title)
	{
		if ( !ImGui::Begin(title) )
		{
			ImGui::End();
			return;
		}

		// Options menu
		if ( ImGui::BeginPopup("Options") )
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
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
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if ( clear )
			Clear();
		if ( copy )
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char *buf = Buf.begin();
		const char *buf_end = Buf.end();
		if ( Filter.IsActive() )
		{
			for ( int line_no = 0; line_no < LineOffsets.Size; line_no++ )
			{
				const char *line_start = buf + LineOffsets[line_no];
				const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				if ( Filter.PassFilter(line_start, line_end) )
					ImGui::TextUnformatted(line_start, line_end);
			}
		}
		else
		{
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while ( clipper.Step() )
			{
				for ( int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++ )
				{
					const char *line_start = buf + LineOffsets[line_no];
					const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if ( AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}

protected:
	void _sink_it(const spdlog::details::log_msg &msg) override
	{
		Buf.append(msg.formatted.c_str());
	}

	void _flush() override
	{
	}
};
}