#include "SaffronPCH.h"

#include "Saffron/Core/TerminalSink.h"

namespace Se
{

ImGuiTextBuffer TerminalSink::m_Buf;
ImGuiTextFilter TerminalSink::m_Filter;
ImVector<int> TerminalSink::m_LineOffsets;
bool TerminalSink::m_AutoScroll;

void TerminalSink::Clear()
{
	m_Buf.clear();
	m_LineOffsets.clear();
	m_LineOffsets.push_back(0);
}

void TerminalSink::Sink(const LogMessage &message)
{
	int old_size = m_Buf.size();
	m_Buf.append(message.Formatted.c_str());
	for ( const int new_size = m_Buf.size(); old_size < new_size; old_size++ )
		if ( m_Buf[old_size] == '\n' )
			m_LineOffsets.push_back(old_size + 1);
}

void TerminalSink::Flush()
{
}
}
