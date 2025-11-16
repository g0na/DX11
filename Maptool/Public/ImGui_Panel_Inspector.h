#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Maptool)

// 현재 선택된 오브젝트나 리소스의 세부 속성을 편집하는 창
class CImGui_Panel_Inspector final : public CImGui_Panel
{
private:
	CImGui_Panel_Inspector();
	virtual ~CImGui_Panel_Inspector();

public:
	virtual HRESULT Initialize() override;
	virtual void Render() override;

private:
	CGameObject*	m_pInspectedObject = { nullptr };

public:
	static CImGui_Panel_Inspector* Create();
	virtual void Free() override;
};

NS_END