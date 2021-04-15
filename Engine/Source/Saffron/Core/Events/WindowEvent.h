#pragma once

#include <sstream>
#include <filesystem>

#include "Saffron/Core/Event.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
class WindowResizedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowResized);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	WindowResizedEvent(int width, int height) :
		m_Width(width),
		m_Height(height)
	{
	}

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

class WindowMovedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMoved);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMovedEvent(const Vector2f& position) :
		m_Position(position)
	{
	}

	const Vector2f& GetPosition() const { return m_Position; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Position: " << m_Position.x << ", " << m_Position.y;
		return oss.str();
	}

private:
	Vector2f m_Position;
};

class WindowGainedFocusEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowGainedFocus);

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

class WindowClosedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowClosed);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	String ToString() const override { return GetName(); }
};

class WindowDroppedFilesEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowDroppedFiles);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowDroppedFilesEvent(ArrayList<Filepath> filepaths) :
		m_Filepaths(Move(filepaths))
	{
	}

	const ArrayList<Filepath>& GetFilepaths() const { return m_Filepaths; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Paths: ";
		for (const auto& filepath : m_Filepaths)
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
	explicit WindowNewTitleEvent(String title) :
		m_Title(Move(title))
	{
	}

	const String& GetTitle() const { return m_Title; }

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
	explicit WindowNewIconEvent(Filepath filepath) :
		m_Filepath(Move(filepath))
	{
	}

	const Filepath& GetFilepath() const { return m_Filepath; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Filepath: " << m_Filepath;
		return oss.str();
	}

private:
	Filepath m_Filepath;
};

class WindowMinimizedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMinimized);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMinimizedEvent(bool restored) :
		m_Restored(restored)
	{
	}

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


class WindowMaximizedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMaximized);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMaximizedEvent(bool restored) :
		m_Restored(restored)
	{
	}

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
