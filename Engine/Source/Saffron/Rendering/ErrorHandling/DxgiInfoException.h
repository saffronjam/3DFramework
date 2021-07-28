#pragma once

#include "Saffron/ErrorHandling/SaffronException.h"

namespace Se
{
class DxgiInfoException : public SaffronException
{
public:
	explicit DxgiInfoException(
		std::vector<std::string> messages,
		std::source_location location = std::source_location::current()
	);

	auto What() const -> std::string override;
	auto Type() const -> const char* override;

};
}
