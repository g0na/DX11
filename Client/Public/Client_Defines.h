#pragma once

#include "windows.h"
#include "process.h"
#include <thread>
#include <mutex>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	// static const unsigned int g_iWinSizeX = { 1280 };
	// static const unsigned int g_iWinSizeY = { 720 };

	static const unsigned int g_iWinSizeX = { 1920 };
	static const unsigned int g_iWinSizeY = { 1080 };

	enum class LEVELID { STATIC, LOADING, LOGO, GAMEPLAY, END};
	enum class TERRAINTEX { DIFFUSE, MASK, END };
}

using namespace std;
using namespace Client;