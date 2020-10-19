#pragma once

#include "Saffron/Gui/Gui.h"
#include "Saffron/Core/LogSink.h"

namespace Se
{
class TerminalSink : public LogSink
{
	friend class EditorTerminal;

public:
	void Clear();

protected:
	void Sink(const LogMessage &message) override;
	void Flush() override;

private:
	static ImGuiTextBuffer m_Buf;
	static ImGuiTextFilter m_Filter;
	static ImVector<int> m_LineOffsets;
	static bool m_AutoScroll;

};
}