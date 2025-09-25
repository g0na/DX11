#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND				hWnd;
		WINMODE				eWinMode;
		unsigned int		iWinSizeX;
		unsigned int		iWinSizeY;
		unsigned int		iLevelNum;
	}EngineDesc;
	
}


#endif // Engine_Struct_h__
