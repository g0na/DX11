#include "Maptool_Defines.h"
#include "ImGui_Panel.h"
#include "GameInstance.h"

CImGui_Panel::CImGui_Panel(const _char* pLabel)
	: m_strLabel(pLabel)
	, m_pGameInstance { CGameInstance::GetInstance() }
{ 
	Safe_AddRef(m_pGameInstance);
}

void CImGui_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
