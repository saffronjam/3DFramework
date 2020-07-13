#define DISABLE_WINWRAPPER

#include "GDIPlusMgr.h"

namespace Gdiplus
{
using std::min;
using std::max;
}

#include <gdiplus.h>

ULONG_PTR GDIPlusMgr::token = 0;
int GDIPlusMgr::refCount = 0;

GDIPlusMgr::GDIPlusMgr()
{
	if ( refCount++ == 0 )
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&token, &input, nullptr);
	}
}

GDIPlusMgr::~GDIPlusMgr()
{
	if ( --refCount == 0 )
	{
		Gdiplus::GdiplusShutdown(token);
	}
}