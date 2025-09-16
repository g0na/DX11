#pragma once

#include "windows.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	static const unsigned int g_iWinSizeX = { 1280 };
	static const unsigned int g_iWinSizeY = { 720 };
}

using namespace std;
using namespace Client;