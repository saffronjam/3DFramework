#pragma once

#include <optional>

#include "VeWin.h"
#include "VeException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "WindowThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

class Window
{
private:
	class Win32Window;

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window& wnd) = delete;
	Window& operator=(const Window& wnd) = delete;

	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages();
public:
	Keyboard kbd;
	Mouse mouse;
	Graphics gfx;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int m_width;
	int m_height;
	HWND m_windowHandle;

private:
	class Win32Window
	{
	public:
		static const char* GetName() { return m_wndClassName; }
		static HINSTANCE GetInstance() { return m_win32Window.m_instanceHandle; }
	private:
		Win32Window() noexcept;
		~Win32Window();
		Win32Window(const Win32Window& win32Window) = delete;
		Win32Window& operator=(const Win32Window& win32Window) = delete;
	private:
		static constexpr const char* m_wndClassName = "V-Engine Window";
		static Win32Window m_win32Window;
		HINSTANCE m_instanceHandle;
	};
public:
	class Exception : public VeException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT m_hr;
	};

};
