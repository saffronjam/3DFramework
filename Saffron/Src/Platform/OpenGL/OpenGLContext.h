#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Se
{
class OpenGLContext : public GraphicsContext
{
public:
	explicit OpenGLContext(GLFWwindow *windowHandle);

	void SwapBuffers() override;
private:
	GLFWwindow *m_WindowHandle;
};
}
