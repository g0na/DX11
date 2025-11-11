#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameObject;
class CCalculator;
NS_END

NS_BEGIN(Maptool)

// 현재 씬의 원본들을 보여주는 창
class CImGui_Panel_Hierarchy final : public CImGui_Panel
{
private:
	CImGui_Panel_Hierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGui_Panel_Hierarchy();

public:
	virtual HRESULT Initialize() override;
	virtual void Render() override;

private:
	_wstring CharToWstring(const _char* pString);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	CGameObject*				m_pSelectedObject = { nullptr };		// 왼쪽 창에서 선택된 원본 오브젝트
	_char						m_szSelectedObj[128] = {};				// 왼쪽 창에서 선택된 원본 오브젝트의 키 값

	CCalculator*				m_pCalculator = { nullptr };

public:
	static CImGui_Panel_Hierarchy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
