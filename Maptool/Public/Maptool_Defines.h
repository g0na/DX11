#pragma once

#include "windows.h"
#include "process.h"
#include <thread>
#include <mutex>
#include <filesystem>
#include <iostream>
#include "GameObject.h"

#pragma push_macro("new")
#undef new
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma pop_macro("new")

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern bool g_bIsCreatable;
extern CGameObject* g_pSelectedPrototype;
extern char g_szSelectedPrototypeName[128];

namespace Maptool
{
	static const unsigned int g_iWinSizeX = { 1280 };
	static const unsigned int g_iWinSizeY = { 720 };

	enum class LEVELID { STATIC, LOADING, LOGO, GAMEPLAY, END};
	enum class TERRAINTEX { DIFFUSE, MASK, END };
}

using namespace std;
using namespace Maptool;