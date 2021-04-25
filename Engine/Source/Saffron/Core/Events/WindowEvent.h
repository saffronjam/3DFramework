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
		_width(width),
		_height(height)
	{
	}

	int GetWidth() const { return _width; }

	int GetHeight() const { return _height; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " " << _width << "x" << _height;
		return oss.str();
	}

private:
	int _width, _height;
};

class WindowMovedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMoved);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMovedEvent(const Vector2& position) :
		_position(position)
	{
	}

	const Vector2& GetPosition() const { return _position; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Position: " << _position.x << ", " << _position.y;
		return oss.str();
	}

private:
	Vector2 _position;
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
		_filepaths(Move(filepaths))
	{
	}

	const ArrayList<Filepath>& GetFilepaths() const { return _filepaths; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Paths: ";
		for (const auto& filepath : _filepaths)
		{
			oss << "[ " << filepath.string() << " ]";
		}
		return oss.str();
	}

private:
	ArrayList<Filepath> _filepaths;
};

class WindowNewTitleEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowNewTitle);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowNewTitleEvent(String title) :
		_title(Move(title))
	{
	}

	const String& GetTitle() const { return _title; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Title: " << _title;
		return oss.str();
	}

private:
	String _title;
};

class WindowNewIconEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowNewIcon);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowNewIconEvent(Filepath filepath) :
		_filepath(Move(filepath))
	{
	}

	const Filepath& GetFilepath() const { return _filepath; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Filepath: " << _filepath;
		return oss.str();
	}

private:
	Filepath _filepath;
};

class WindowMinimizedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMinimized);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMinimizedEvent(bool restored) :
		_restored(restored)
	{
	}

	bool Restored() const { return _restored; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Minimize Restored: " << _restored;
		return oss.str();
	}

private:
	bool _restored;
};


class WindowMaximizedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMaximized);

	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMaximizedEvent(bool restored) :
		_restored(restored)
	{
	}

	bool Restored() const { return _restored; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Maximize Restored: " << _restored;
		return oss.str();
	}

private:
	bool _restored;
};
}
