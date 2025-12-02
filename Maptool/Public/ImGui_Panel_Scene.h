#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CCalculator;
class CGameObject;
NS_END

NS_BEGIN(Maptool)

class CImGui_Panel_Scene final : public CImGui_Panel
{
public:
	typedef struct tagJSONGameObjcetDesc
	{
		string strPrototypeTag = {};
		string strLayerTag = {};
		_float3 vPosition = {};
		_float3 vRotation = {};
		_float3 vScale = {};
	}JSONGAMEOBJECT_DESC;

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
	string	WideToMultiByte(const _wstring& wstr);

	HRESULT Save_Mapdata();
	HRESULT Load_Mapdata(const _tchar* pFilePath);
	void to_json(ordered_json& j, const JSONGAMEOBJECT_DESC& jsonDesc);
	void from_json(const ordered_json& j, JSONGAMEOBJECT_DESC& jsonDesc);

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CCalculator*						m_pCalculator = { nullptr };
	CGameObject*						m_pSelectedObject = { nullptr };
	class CImGui_Panel_Inspector*		m_pInspector = { nullptr };

	_wstring							m_strJsonPath = {};
	_float3								m_vRotationAngle{};

public:
	static CImGui_Panel_Scene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END