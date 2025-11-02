#include "Client_Defines.h"
#include "ImGui_Panel.h"

CImGui_Panel::CImGui_Panel(const _char* pLabel)
	: m_strLabel(pLabel)
{
}

void CImGui_Panel::Free()
{
	__super::Free();
}
