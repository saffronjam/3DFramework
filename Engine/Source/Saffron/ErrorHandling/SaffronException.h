#pragma once

#include <source_location>
#include <string>
#include <sstream>
#include <filesystem>

#include "Saffron/Base.h"

namespace Se
{
class SaffronException : public std::exception
{
public:
	explicit SaffronException(std::source_location location) :
		_location(std::move(location))
	{
	}

	SaffronException(std::string message, std::source_location location) :
		_location(std::move(location)),
		_message(std::move(message))
	{
	}

	[[nodiscard]] virtual auto What() const -> std::string;

	virtual auto Type() const -> const char*;

protected:
	template <class T>
	constexpr auto Prefix() const -> std::string;

	template <class T>
	constexpr auto Formatted() const -> std::string;

private:
	char const* what() const override;

protected:
	std::source_location _location;
	mutable std::string _message;

private:
	mutable std::string _buffer;
};

template <class T>
constexpr auto SaffronException::Formatted() const -> std::string
{
	std::filesystem::path path(_location.file_name());

	std::string pathString = path.string();
	pathString = pathString.substr(pathString.find("Source\\Saffron") + 7);


	std::ostringstream oss;
	oss << Prefix<T>() << '\n';
	oss << pathString << ':' << _location.line() << " (" << _location.function_name() << ")\n";
	oss << "\n" << _message << '\n';
	return oss.str();
}

template <class T>
constexpr auto SaffronException::Prefix() const -> std::string
{
	std::ostringstream oss;
	oss << "[" << Type() << "]";
	return oss.str();
}
}
