#pragma once

#include "Saffron/Core/TerminalSink.h"

namespace Se
{
class EditorTerminal
{
public:
	EditorTerminal();

	void Clear();
	void OnGuiRender();

	void SetLevel(Log::Level::LevelEnum level);

private:
	Shared<TerminalSink> m_Sink;
	ImGuiTextFilter m_Filter;
	bool m_AutoScroll = false;
};
}