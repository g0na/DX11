#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CCalculator;
class CGameObject;
NS_END

NS_BEGIN(Maptool)

class CImGui_Panel_Scene final : public CImGui_Panel
{
private:
	CImGui_Panel_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGui_Panel_Scene();

public:
	void Set_Inspector(class CImGui_Panel_Inspector* pInspector) { m_pInspector = pInspector; }
	void Set_SelectedObject(CGameObject* pSelectedObject) { m_pSelectedObject = pSelectedObject; }

public:
	virtual HRESULT Initialize() override;
	virtual void Render() override;

private:
	_wstring CharToWstring(const _char* pString);

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CCalculator*						m_pCalculator = { nullptr };
	CGameObject*						m_pSelectedObject = { nullptr };
	class CImGui_Panel_Inspector*		m_pInspector = { nullptr };

public:
	static CImGui_Panel_Scene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END