#pragma once

#include "windows.h"
#include "process.h"
#include <thread>
#include <mutex>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Maptool
{
	static const unsigned int g_iWinSizeX = { 1600 };
	static const unsigned int g_iWinSizeY = { 900 };

	enum class LEVELID { STATIC, LOADING, LOGO, GAMEPLAY, END};
	enum class TERRAINTEX { DIFFUSE, MASK, END };
}

using namespace std;
using namespace Maptool;