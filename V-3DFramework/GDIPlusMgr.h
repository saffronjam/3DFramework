#pragma once

#include <algorithm>

#include "VeWin.h"

class GDIPlusMgr
{
public:
	GDIPlusMgr();
	~GDIPlusMgr();

private:
	static ULONG_PTR token;
	static int refCount;
};