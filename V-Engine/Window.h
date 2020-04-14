#pragma once

#include "VeWin.h"
#include "VeException.h"

namespace ve
{
class Window
{
public:
	class Exception : public VeException
	{
	public:
		Exception(int line, const char *file, HRESULT hr) noexcept;
		const char *what() const noexcept override;
		virtual const char *GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT m_hr;
	};
private:
	class WindowClass
	{
	public:
		static auto GetName() { return m_wndClassName; }
		static auto GetInstance() { return m_wndClass.m_hInstance; }
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass &wndClass) = delete;
		WindowClass &operator=(const WindowClass &wndClass) = delete;
	private:
		static constexpr const char *m_wndClassName = "V-Engine Window";
		static WindowClass m_wndClass;
		HINSTANCE m_hInstance;
	};
public:
	Window(int width, int height, const char *name);
	~Window();
	Window(const Window &wnd) = delete;
	Window &operator=(const Window &wnd) = delete;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int m_width;
	int m_height;
	HWND m_hWnd;

};
}

#define VEWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr);