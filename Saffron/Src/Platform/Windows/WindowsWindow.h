#pragma once

#include "Saffron/Graphics/Window.h"
#include "Saffron/Event/WindowEvent.h"
#include "Saffron/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const WindowProps &props);
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
	Scope<GraphicsContext> m_Context;
	static bool m_sGLFWInitialized;
	bool m_VSync;
};
}
