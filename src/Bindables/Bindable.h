#pragma once

#include <GL/glew.h>

#include "Graphics.h"
#include "GenericThrowMacros.h"

class Bindable
{
public:
    virtual void BindTo(Graphics &gfx) = 0;

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


