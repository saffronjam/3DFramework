#pragma once

#include "Saffron/Common/Window.h"
#include "Saffron/Event/Event.h"

struct GLFWwindow;
struct GLFWcursor;

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const WindowSpec& spec);
	~WindowsWindow() override;

	void OnUpdate() override;

	void* NativeHandle() const override;

private:
	void SetupGLFWCallbacks();

	void Restore();

private:
	GLFWwindow* m_NativeWindow;
	GLFWcursor* m_ImGuiMouseCursors[9] = {nullptr};

	bool m_VSync, m_Minimized, m_Maximized;
};
}
