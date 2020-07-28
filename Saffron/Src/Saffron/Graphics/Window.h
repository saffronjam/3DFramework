#pragma once

#include "Saffron/Config.h"
#include "Saffron/SaffronPCH.h"
#include "Saffron/Event/WindowEvent.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/System/Log.h"

namespace Se
{
class Window : public EventHandler
{
public:
	using Ptr = std::shared_ptr<Window>;

public:
	Window(std::string title, int width, int height, bool activeContext = true);
	virtual ~Window();

	void OnUpdate();

	bool ShouldClose();
	void RequestClose();
	void RequestFocus();

	template<typename T, typename...Params>
	void PushEvent(Params &&...params);
	void HandleBufferedEvents();
	void AddEventHandler(EventHandler *handler);
	void OnEvent(const Event::Ptr &pEvent) override;

	struct GLFWwindow *GetCoreWindow() const;
	const glm::vec2 &GetPosition() const;
	int GetWidth() const;
	int GetHeight() const;

	bool HasFocus() const;

	void SetTitle(std::string title);
	void SetVSync(bool enabled);

private:
	void SetupGLFWCallbacks();

	void OnResize(const WindowResizeEvent::Ptr &event);
	void OnMove(const WindowMoveEvent::Ptr &event);
	void OnGainFocus(const WindowGainFocusEvent::Ptr &event);
	void OnLostFocus(const WindowLostFocusEvent::Ptr &event);
	void OnClose(const WindowCloseEvent::Ptr &event);

private:
	std::string m_title;
	int m_width, m_height;
	glm::vec2 m_position;
	bool m_hasFocus;

	struct GLFWwindow *m_nativeWindowHandle;

	std::vector<Event::Ptr> m_events;
	std::set<EventHandler *> m_handlers;

	static bool m_sInitialized;
	static bool m_sAnyActiveContext;

public:
	Keyboard kbd;
	Mouse mouse;
};

template<typename T, typename...Params>
void Window::PushEvent(Params &&...params)
{
	m_events.emplace_back(std::make_shared<T>(std::forward<Params>(params)...));
	SE_INFO("{0}", m_events.back()->ToString());
}

}
