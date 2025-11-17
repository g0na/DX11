#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Maptool)

// 현재 씬의 원본들을 보여주는 창
class CImGui_Panel_Prototype final : public CImGui_Panel
{
private:
	CImGui_Panel_Prototype();
	virtual ~CImGui_Panel_Prototype();

public:
	virtual HRESULT Initialize() override;
	virtual void Render() override;

private:
	CGameObject*						m_pSelectedObject = { nullptr };		// 왼쪽 창에서 선택된 원본 오브젝트
	_char								m_szSelectedObj[128] = {};				// 왼쪽 창에서 선택된 원본 오브젝트의 키 값

public:
	static CImGui_Panel_Prototype* Create();
	virtual void Free() override;
};

NS_END
