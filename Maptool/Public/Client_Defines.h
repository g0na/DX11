#pragma once

#include "windows.h"
#include "process.h"
#include <thread>
#include <mutex>

#pragma push_macro("new")
#undef new
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma pop_macro("new")

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Maptool
{
	static const unsigned int g_iWinSizeX = { 1280 };
	static const unsigned int g_iWinSizeY = { 720 };

	enum class LEVELID { STATIC, LOADING, LOGO, GAMEPLAY, END};
	enum class TERRAINTEX { DIFFUSE, MASK, END };
}

using namespace std;
using namespace Maptool;




/*
1. Github에서 ImGui 가져오기 총 15개 파일
(바로있는거 11개, backends폴더에 win32, direct11 h, cpp 4개)

2. 96. ImGui 생성 후 Default Filter에 박아두기

3. Define에 선언 (new 매크로 충돌 뽀인또)
	빌드 성공시 초기화 진행

4. ImGuiManager 생성 Client에 -> Engine에서 관리한다면 Engine으로 뺴야하는것 언급

5. Ready_ToolGara 추가해서 분리해두기

6. m_pDevice, m_pDeviceContext를 넘겨서 GuiManager 내부에서 초기화 진행

7. 진행 후 ImGui_Panle 클래스 후다닥 만들기

8. ImGui Demo 초반에 쓰면 좋다는거 ImGuiDemo.cpp에서 기능 확인해보고 내가 뜯어오기 등

9. 그런데 지금 이상태로 쓰면 먹통이된다!

10. ImGui_ImplWin32_WndProcHandler 추가

11. 이제 이렇게 되면 ImGui가 항상 살아있게 되고, Scene에 국한되게 되면 ImGui는 파괴되는것이 아닌 잠깐 사라졌따가 다시 켜지는것
다시 켜졌을때 데이터들이 살아있게 된다면 Dangling Pointer의 위험이 있기 때문에 고민해보아야할 문제
그래서 나는 깔끔하게 Tool Project를 따로 파서 진행한다.

12. GameInstance를 들어가서 Render 구조 확인 LevelManager의 Render에 넣을건데, Object를 그리고 난뒤에 ImGui를 Render하는게 바람직하다.

13. Create를 해보자, Monster의 ModelComponent Prototype_Component_Model_Fiona 로 변경 후 생성 버튼

14. Button을 if문으로 감싼것 언급

15. 그냥 여러 오브젝트 생성해서 디버깅으로 Layer에서 몇마리 생성했는지 보여주고

16. LightDesc 가져와서 Button이나 InputFloat을 통해서 조정해주기

17. 한쪽엔 버튼 한쪽엔 input float

18. 배치하는것들 알려주고 끝
*/
