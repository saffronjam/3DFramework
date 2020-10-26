#pragma once

#include <utility>
#include <optional>

#include "Saffron/Base.h"
#include "Saffron/Core/Events/WindowEvent.h" 
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
// Interface representing a desktop system based Window
class Window : public ReferenceCounted
{
public:
	using EventCallback = Function<void(const Event &)>;

public:
	struct Properties
	{
		String Title;
		Uint32 Width;
		Uint32 Height;
		Vector2f Position;

		explicit Properties(String title = "Saffron Engine",
							Uint32 width = 1280,
							Uint32 height = 720,
							const Vector2f position = { 100.0f, 100.0f })
			: Title(Move(title)), Width(width), Height(height), Position(position)
		{
		}
	};

public:
	Window(const Properties &properties);
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
	virtual const Vector2f &GetPosition() const;
	virtual void *GetNativeWindow() const = 0;

	// Window attributes
	void SetTitle(String title);
	void SetWindowIcon(Filepath filepath);
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;
	virtual bool IsMinimized() const = 0;

	static Shared<Window> Create(const Properties &properties = Properties());

protected:
	String m_Title;
	Vector2f m_Position;
	Uint32 m_Width, m_Height;

private:
	ArrayList<Shared<Event>> m_Events;
	std::optional<EventCallback> m_EventCallback;
};

template<typename T, typename...Params>
void Window::PushEvent(Params &&...params)
{
	m_Events.emplace_back(Shared<T>::Create(std::forward<Params>(params)...));
	//SE_INFO("{0}", m_Events.back()->ToString());
}

};
