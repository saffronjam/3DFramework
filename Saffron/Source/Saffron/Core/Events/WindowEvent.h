#pragma once

#include <sstream>
#include <utility>
#include <filesystem>

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Event.h"
#include "Saffron/Renderer/AntiAliasing.h"

namespace Se
{
class WindowResizeEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowResize);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	WindowResizeEvent(int width, int height) : m_Width(width), m_Height(height) {}

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " " << m_Width << "x" << m_Height;
		return oss.str();
	}

private:
	int m_Width, m_Height;
};

class WindowMoveEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMove);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMoveEvent(const Vector2f &position) : m_Position(position) {}

	const Vector2f &GetPosition() const { return m_Position; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Position: " << m_Position.x << ", " << m_Position.y;
		return oss.str();
	}

private:
	Vector2f m_Position;
};

class WindowGainFocusEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowGainFocus);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	String ToString() const override { return GetName(); }

};

class WindowLostFocusEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowLostFocus);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	String ToString() const override { return GetName(); }

};

class WindowCloseEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowClose);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	String ToString() const override { return GetName(); }

};

class WindowDropFilesEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowDropFiles);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowDropFilesEvent(ArrayList<Filepath> filepaths) : m_Filepaths(Move(filepaths)) {}

	const ArrayList<Filepath> &GetFilepaths() const { return m_Filepaths; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Paths: ";
		for ( const auto &filepath : m_Filepaths )
		{
			oss << "[ " << filepath.string() << " ]";
		}
		return oss.str();
	}

private:
	ArrayList<Filepath> m_Filepaths;
};

class WindowNewTitleEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowNewTitle);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowNewTitleEvent(String title) : m_Title(Move(title)) {}

	const String &GetTitle() const { return m_Title; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Title: " << m_Title;
		return oss.str();
	}

private:
	String m_Title;
};

class WindowNewIconEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowNewIcon);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowNewIconEvent(Filepath filepath) : m_Filepath(Move(filepath)) {}

	const Filepath &GetFilepath() const { return m_Filepath; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Filepath: " << m_Filepath;
		return oss.str();
	}

private:
	Filepath m_Filepath;
};

class WindowNewAntiAliasingEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowNewAntiAliasing);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowNewAntiAliasingEvent(AntiAliasing antiAliasing) : m_AntiAliasing(antiAliasing) {}

	const AntiAliasing &GetAntiAliasing() const { return m_AntiAliasing; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " AntiAliasing: " << static_cast<int>(m_AntiAliasing);
		return oss.str();
	}

private:
	AntiAliasing m_AntiAliasing;
};

class WindowMinimizeEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMinimize);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMinimizeEvent(bool restored) : m_Restored(restored) {}

	bool Restored() const { return m_Restored; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Minimize Restored: " << m_Restored;
		return oss.str();
	}

private:
	bool m_Restored;
};


class WindowMaximizeEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMaximize);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMaximizeEvent(bool restored) : m_Restored(restored) {}

	bool Restored() const { return m_Restored; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Maximize Restored: " << m_Restored;
		return oss.str();
	}

private:
	bool m_Restored;
};


}