#include "SaffronPCH.h"

#include "Saffron/Rendering/ErrorHandling/DxgiInfoException.h"

namespace Se
{
DxgiInfoException::DxgiInfoException(std::vector<std::string> messages, std::source_location location) :
	SaffronException(location)
{
	if (messages.size() > 1)
	{
		std::ostringstream oss;
		oss << messages.size() << " messages: ";
		for (const auto& message : messages)
		{
			oss << "\n\n" << message;
		}
		_message = oss.str();
	}
	else if (messages.size() == 1)
	{
		_message = messages.front();
	}
	else
	{
		_message = "Unknown error";
	}
}

auto DxgiInfoException::What() const -> std::string
{
	return Formatted<DxgiInfoException>();
}

auto DxgiInfoException::Type() const -> const char*
{
	return "DXGI Info Exception";
}
}
