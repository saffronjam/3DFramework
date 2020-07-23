#include "GLCheck.h"

#include <string>

#include <GL/glew.h>

#include "Log.h"

void SaffronGL::CheckError(const char *file, unsigned int line, const char *expression)
{
    // Get the last error
    GLenum errorCode = glGetError();

    if (errorCode != GL_NO_ERROR)
    {
        std::string fileString = file;
        std::string error = "Unknown error";
        std::string description = "No description";

        // Decode the error code
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
        {
            error = "GL_INVALID_ENUM";
            description = "An unacceptable value has been specified for an enumerated argument.";
            break;
        }

        case GL_INVALID_VALUE:
        {
            error = "GL_INVALID_VALUE";
            description = "A numeric argument is out of range.";
            break;
        }

        case GL_INVALID_OPERATION:
        {
            error = "GL_INVALID_OPERATION";
            description = "The specified operation is not allowed in the current state.";
            break;
        }

        case GL_STACK_OVERFLOW:
        {
            error = "GL_STACK_OVERFLOW";
            description = "This command would cause a stack overflow.";
            break;
        }

        case GL_STACK_UNDERFLOW:
        {
            error = "GL_STACK_UNDERFLOW";
            description = "This command would cause a stack underflow.";
            break;
        }

        case GL_OUT_OF_MEMORY:
        {
            error = "GL_OUT_OF_MEMORY";
            description = "There is not enough memory left to execute the command.";
            break;
        }

        case GL_INVALID_FRAMEBUFFER_OPERATION:
        {
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
            break;
        }
        default:
            break;
        }
        LogWarningUser("GLCheck caught a bad operation: \"%s\"\n\t[Problem]: %s\n\t[File]: %s:%u\n", expression, std::string(error + " " + description).c_str(), fileString.substr(fileString.find_last_of("\\/") + 1).c_str(), line);
        //throw SaffronGL::Exception(static_cast<int>(line), fileString.substr(fileString.find_last_of("\\/") + 1).c_str(), expression, std::string(error + " " + description).c_str());

    }
}

SaffronGL::Exception::Exception(int line, const char *file, const char *expression, const char *errorString) noexcept
        : IException(line, file),
          expression(expression),
          errorString(errorString)
{
}

const char *SaffronGL::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Expression] " << GetExpression() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *SaffronGL::Exception::GetType() const noexcept
{
    return "Saffron GL Exception";
}

const char *SaffronGL::Exception::GetExpression() const noexcept
{
    return expression.c_str();
}

const char *SaffronGL::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}
