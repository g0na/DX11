#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Maptool)

// 현재 씬의 오브젝트 계층을 트리 형태로 보여주는 창
class CImGui_Panel_Hierarchy final : public CImGui_Panel
{
private:
	CImGui_Panel_Hierarchy();
	virtual ~CImGui_Panel_Hierarchy();
public:
	virtual HRESULT Intiailzie() override;
	virtual void Render() override;
private:

public:
	static CImGui_Panel_Hierarchy* Create();
	virtual void Free() override;
};

NS_END
