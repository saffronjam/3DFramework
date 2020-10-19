#pragma once

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");

#include "Saffron/Core/TerminalSink.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
class EditorTerminal
{
public:
	static void Init();

	static void Clear();
	static void OnGuiRender();

	static void SetLevel(Log::Level::LevelEnum level);

private:
	std::shared_ptr<TerminalSink> m_Sink;

};
}