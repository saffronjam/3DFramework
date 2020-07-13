#include "VeException.h"

VeException::VeException(int line, const char *file) noexcept
	:
	m_line(line),
	m_file(file)
{
}

const char *VeException::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *VeException::GetType() const noexcept
{
	return "VeException";
}

std::string VeException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl
		<< "[Line] " << m_line << std::endl;
	return oss.str();
}
