#include "SaffronPCH.h"

#include "Saffron/Core/TerminalSink.h"

namespace Se
{
void TerminalSink::Clear()
{
	m_TextBuffer.clear();
	m_LineOffsets.clear();
	m_LineOffsets.push_back(0);
}

void TerminalSink::Sink(const LogMessage& message)
{
	int old_size = m_TextBuffer.size();
	m_TextBuffer.append(message.Formatted.c_str());
	for (const int new_size = m_TextBuffer.size(); old_size < new_size; old_size++) if (m_TextBuffer[old_size] == '\n')
		m_LineOffsets.push_back(old_size + 1);
}

void TerminalSink::Flush()
{
}
}
