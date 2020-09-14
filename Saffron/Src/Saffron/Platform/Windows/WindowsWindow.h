#pragma once

#include "Saffron/Graphics/Window.h"
#include "Saffron/Core/Event/WindowEvent.h"

struct GLFWwindow;
struct GLFWcursor;

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const Properties &props);
	virtual ~WindowsWindow();

	void OnUpdate() override;
	void OnEvent(const Event &event) override;

	void Close() override;
	void Focus() override;

	void *GetNativeWindow() const override;

	// Window attributes
	void SetTitle(std::string title) override;
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

private:
	void OnResize(const WindowResizeEvent &event);
	void OnMove(const WindowMoveEvent &event);
	void OnGainFocus(const WindowGainFocusEvent &event);
	void OnLostFocus(const WindowLostFocusEvent &event);
	void OnClose(const WindowCloseEvent &event);

	void SetupGLFWCallbacks();

private:
	GLFWwindow *m_NativeWindow;
	GLFWcursor *m_ImGuiMouseCursors[9] = { nullptr };

	bool m_VSync;
};
}
