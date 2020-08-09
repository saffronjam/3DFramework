#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Core/Event/WindowEvent.h" 
#include "Saffron/System/SaffronMath.h"

namespace Se
{
struct WindowProps
{
	std::string Title;
	Uint32 Width;
	Uint32 Height;
	glm::vec2 Position;

	explicit WindowProps(const std::string &title = "Saffron Engine",
						 Uint32 width = 1280,
						 Uint32 height = 720,
						 const glm::vec2 position = { 100.0f, 100.0f })
		: Title(title), Width(width), Height(height), Position(position)
	{
	}
};

// Interface representing a desktop system based Window
class Window
{
public:
	using EventCallback = std::function<void(const Event &)>;

public:
	Window();
	virtual ~Window() = default;

	virtual void OnUpdate() = 0;
	virtual void OnEvent(const Event &event) {}

	virtual void Close() = 0;
	virtual	void Focus() = 0;

	template<typename T, typename...Params>
	void PushEvent(Params &&...params);
	void HandleBufferedEvents();
	void SetEventCallback(const EventCallback &callback);

	virtual Uint32 GetWidth() const;
	virtual Uint32 GetHeight() const;
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
	Uint32 m_Width, m_Height;

private:
	std::vector<Ref<Event>> m_Events;
	std::optional<EventCallback> m_EventCallback;
};

template<typename T, typename...Params>
void Window::PushEvent(Params &&...params)
{
	m_Events.emplace_back(std::make_shared<T>(std::forward<Params>(params)...));
	//SE_INFO("{0}", m_Events.back()->ToString());
}

};
