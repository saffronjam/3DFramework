#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Events/KeyboardEvent.h"
#include "Saffron/Common/Events/MouseEvent.h"
#include "Saffron/Common/Events/WindowEvent.h"
#include "Saffron/Common/EventSubscriberList.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/AntiAliasing.h"

namespace Se
{
// Interface representing a desktop system based Window
class Window : public MemManaged<Window>
{
	friend class Mouse;

public:
	struct Properties
	{
		explicit Properties(String title = "Saffron Engine", Uint32 width = 1280, Uint32 height = 720,
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
		Uint32 Width;
		Uint32 Height;
		Vector2 Position;
		AntiAliasing AntiAliasing;
	};

public:
	explicit Window(const Properties& properties);
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

	virtual Uint32 GetWidth() const;
	virtual Uint32 GetHeight() const;
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

	static Shared<Window> Create(const Properties& properties);

private:
	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual bool IsMinimized() const = 0;
	virtual bool IsMaximized() const = 0;

	virtual void EnableCursor() = 0;
	virtual void DisableCursor() = 0;

	virtual void ShowCursor() = 0;
	virtual void HideCursor() = 0;

	virtual void ConfineCursor() = 0;
	virtual void FreeCursor() = 0;

public:
	// Window events
	mutable EventSubscriberList<WindowResizedEvent> Resized;
	mutable EventSubscriberList<WindowMinimizedEvent> Minimized;
	mutable EventSubscriberList<WindowMaximizedEvent> Maximized;
	mutable EventSubscriberList<WindowMovedEvent> Moved;
	mutable EventSubscriberList<WindowGainedFocusEvent> GainedFocus;
	mutable EventSubscriberList<WindowLostFocusEvent> LostFocus;
	mutable EventSubscriberList<WindowClosedEvent> Closed;
	mutable EventSubscriberList<WindowDroppedFilesEvent> DroppedFiles;
	mutable EventSubscriberList<WindowNewTitleEvent> NewTitle;

	// Keyboard events
	mutable EventSubscriberList<KeyPressedEvent> KeyPressed;
	mutable EventSubscriberList<KeyReleasedEvent> KeyReleased;
	mutable EventSubscriberList<KeyRepeatedEvent> KeyRepeated;
	mutable EventSubscriberList<TextInputEvent> TextInput;

	// Mouse events
	mutable EventSubscriberList<MouseButtonPressedEvent> MouseButtonPressed;
	mutable EventSubscriberList<MouseButtonReleasedEvent> MouseButtonReleased;
	mutable EventSubscriberList<MouseWheelScrolledEvent> MouseWheelScrolled;
	mutable EventSubscriberList<MouseMovedEvent> MouseMoved;
	mutable EventSubscriberList<MouseMovedRawEvent> MouseMovedRaw;
	mutable EventSubscriberList<CursorEnteredEvent> CursorEntered;
	mutable EventSubscriberList<CursorLeftEvent> CursorLeft;

protected:
	String _title;
	Vector2 _position;
	Uint32 _width, _height;
	AntiAliasing _antiAliasing;

private:
	ArrayList<Shared<Event>> _events;
};

template <typename T, typename...Params>
void Window::PushEvent(Params&&...params)
{
	_events.emplace_back(CreateShared<T>(std::forward<Params>(params)...));
}
};
