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
	
	typedef struct tagVertexPositionCoord
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexcoord;
	}VTXPOSTEX;

}


#endif // Engine_Struct_h__
