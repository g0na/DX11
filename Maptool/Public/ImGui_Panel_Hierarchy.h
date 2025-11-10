#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Maptool)

// 현재 씬의 원본들을 보여주는 창
class CImGui_Panel_Hierarchy final : public CImGui_Panel
{
private:
	CImGui_Panel_Hierarchy();
	virtual ~CImGui_Panel_Hierarchy();
public:
	virtual HRESULT Initialize() override;
	virtual void Render() override;

private:
	_char	m_szSelectedObj[128] = {};

public:
	static CImGui_Panel_Hierarchy* Create();
	virtual void Free() override;
};

NS_END
