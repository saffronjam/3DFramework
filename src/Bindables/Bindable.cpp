#include "Bindable.h"

Bindable::Exception::Exception(int line, const char *file, const char *errorString) noexcept
        : IException(line, file),
          errorString(errorString)
{
}

const char *Bindable::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Bindable::Exception::GetType() const noexcept
{
    return "Saffron Bindable Exception";
}

const char *Bindable::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}