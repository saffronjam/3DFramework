#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/System/SaffronMath.h"

// TEMPORARY
#include "Saffron/System/Log.h"

namespace Se
{
struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;
	glm::vec2 Position;

	explicit WindowProps(const std::string &title = "Saffron Engine",
						 unsigned int width = 1280,
						 unsigned int height = 720,
						 const glm::vec2 position = { 100.0f, 100.0f })
		: Title(title), Width(width), Height(height), Position(position)
	{
	}
};

// Interface representing a desktop system based Window
class Window : public EventHandler
{
public:
	Window();
	virtual ~Window() = default;

	virtual void OnUpdate() = 0;
	void OnEvent(const Event &event) override {};

	virtual void Close() = 0;
	virtual	void Focus() = 0;

	template<typename T, typename...Params>
	void PushEvent(Params &&...params);
	void AddEventHandler(EventHandler *handler);
	void HandleBufferedEvents();

	virtual uint32_t GetWidth() const;
	virtual uint32_t GetHeight() const;
	virtual const glm::vec2 &GetPosition() const;
	virtual void *GetNativeWindow() const = 0;

	// Window attributes
	virtual void SetTitle(std::string title) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;


	static Ref<Window> Create(const WindowProps &props = WindowProps());

protected:
	std::string m_Title;
	glm::vec2 m_Position;
	unsigned int m_Width, m_Height;

private:
	std::vector<Ref<Event>> m_events;
	std::set<EventHandler *> m_handlers;
};

template<typename T, typename...Params>
void Window::PushEvent(Params &&...params)
{
	m_events.emplace_back(std::make_shared<T>(std::forward<Params>(params)...));
	SE_INFO("{0}", m_events.back()->ToString());
}

};


