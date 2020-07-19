#pragma once

#include <GL/glew.h>

#include "Graphics.h"
#include "GenericThrowMacros.h"
#include "GLCheck.h"
#include "NonCopyable.h"

class Bindable : public NonCopyable
{
public:
    Bindable();
    virtual ~Bindable() = default;
    Bindable(Bindable &&bindable) noexcept;

    virtual void BindTo(Graphics &gfx) = 0;

protected:
    GLuint m_GLResourceID;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *errorString) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        const char *GetErrorString() const noexcept;

    private:
        std::string errorString;
    };

};


