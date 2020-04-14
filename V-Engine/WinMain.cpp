#include "Window.h"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	try
	{
		ve::Window window(800, 300, "V-Engine");

		MSG msg;
		BOOL gResult;
		while ( (gResult = GetMessage(&msg, nullptr, 0, 0)) > 0 )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if ( gResult == -1 )
			return -1;

		return (int)msg.wParam;
	}
	catch ( const VeException &e )
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( const std::exception &e )
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( ... )
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}