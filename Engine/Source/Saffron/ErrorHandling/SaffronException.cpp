#include "SaffronPCH.h"

#include "Saffron/ErrorHandling/SaffronException.h"

namespace Se
{
auto SaffronException::What() const -> std::string
{
	return Formatted<SaffronException>();
}

auto SaffronException::Type() const -> const char*
{
	return "Saffron Exception";
}

char const* SaffronException::what() const
{
	_buffer = What();
	return _buffer.c_str();
}
}
