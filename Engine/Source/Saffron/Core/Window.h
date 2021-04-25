#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/AntiAliasing.h"

namespace Se
{
// Interface representing a desktop system based Window
class Window : public Managed
{
public:
	struct Properties
	{
		explicit Properties(String title = "Saffron Engine", uint width = 1280, uint height = 720,
		                    const Vector2 position = {100.0f, 100.0f},
		                    AntiAliasing antiAliasing = AntiAliasing::Sample8) :
			Title(Move(title)),
			Width(width),
			Height(height),
			Position(position),
			AntiAliasing(antiAliasing)
		{
		}

		String Title;
		uint Width;
		uint Height;
		Vector2 Position;
		AntiAliasing AntiAliasing;
	};

public:
	Window(const Properties& properties);
	virtual ~Window() = default;

	virtual void OnUpdate() = 0;

	virtual void OnEvent(const Event& event)
	{
	}

	virtual void Close() = 0;
	virtual void Focus() = 0;

	template <typename T, typename...Params>
	void PushEvent(Params&&...params);
	void HandleBufferedEvents();

	virtual uint GetWidth() const;
	virtual uint GetHeight() const;
	virtual const Vector2& GetPosition() const;
	virtual void* GetNativeWindow() const = 0;

	// Window attributes
	void SetTitle(String title);
	const String& GetTitle() const;
	void SetWindowIcon(Filepath filepath);
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;

	void SetAntiAliasing(AntiAliasing antiAliasing);
	AntiAliasing GetAntiAliasing() const;

	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual bool IsMinimized() const = 0;
	virtual bool IsMaximized() const = 0;

	static Shared<Window> Create(const Properties& properties = Properties());

public:
	// Window events
	mutable EventSubscriberList<const WindowResizedEvent&> Resized;
	mutable EventSubscriberList<const WindowMinimizedEvent&> Minimized;
	mutable EventSubscriberList<const WindowMaximizedEvent&> Maximized;
	mutable EventSubscriberList<const WindowMovedEvent&> Moved;
	mutable EventSubscriberList<const WindowGainedFocusEvent&> GainedFocus;
	mutable EventSubscriberList<const WindowLostFocusEvent&> LostFocus;
	mutable EventSubscriberList<const WindowClosedEvent&> Closed;
	mutable EventSubscriberList<const WindowDroppedFilesEvent&> DroppedFiles;
	mutable EventSubscriberList<const WindowNewTitleEvent&> NewTitle;
	mutable EventSubscriberList<const WindowNewIconEvent&> NewIcon;

	// Keyboard events
	mutable EventSubscriberList<const KeyPressedEvent&> KeyPressed;
	mutable EventSubscriberList<const KeyReleasedEvent&> KeyReleased;
	mutable EventSubscriberList<const KeyRepeatedEvent&> KeyRepeated;
	mutable EventSubscriberList<const TextInputEvent&> TextInput;

	// Mouse events
	mutable EventSubscriberList<const MouseButtonPressedEvent&> MouseButtonPressed;
	mutable EventSubscriberList<const MouseButtonReleasedEvent&> MouseButtonReleased;
	mutable EventSubscriberList<const MouseWheelScrolledEvent&> MouseWheelScrolled;
	mutable EventSubscriberList<const MouseMovedEvent&> MouseMoved;
	mutable EventSubscriberList<const MouseMovedRawEvent&> MouseMovedRaw;
	mutable EventSubscriberList<const CursorEnteredEvent&> CursorEntered;
	mutable EventSubscriberList<const CursorLeftEvent&> CursorLeft;

protected:
	String _title;
	Vector2 _position;
	uint _width, _height;
	AntiAliasing _antiAliasing;

private:
	ArrayList<Shared<Event>> _events;
};

template <typename T, typename...Params>
void Window::PushEvent(Params&&...params)
{
	_events.emplace_back(Shared<T>::Create(std::forward<Params>(params)...));
	//Log::Info("{0}", _events.back()->ToString());
}
};
