#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_GamePlay : public CLevel
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
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_UIs(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const _wstring& strLayerTag);

private:
	_wstring	CharToWstring(const _char* pString);
	HRESULT		Load_Mapdata(const _tchar* pFilePath);
	void		from_json(const ordered_json& j, JSONGAMEOBJECT_DESC& jsonDesc);

private:
	_wstring		m_strJsonPath = {};
	_float3			m_vRotationAngle{};

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END