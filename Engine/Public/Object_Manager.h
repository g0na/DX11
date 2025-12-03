#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	explicit CObject_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iLevelNum);
	void	Update_Priority(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Update_Late(_float fTimeDelta);
	void	Clear(_uint iLevelNum);

public:
	// 특정 레벨의 특정 원본을, 특정 레벨의 특정 레이어에 추가하는 함수
	class CGameObject* Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strProtoTag,
		_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);

	void Delete_GameObject_From_Layer(class CGameObject* pGameObject, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	map<const _wstring, class CLayer*>* Get_Layers() const { return m_pLayers; }
	class CGameObject* Get_Player(_uint iLevelIndex);

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	class CGameInstance*			m_pGameInstance = { nullptr };

	map<const _wstring, class CLayer*>*		m_pLayers = { nullptr };
	_uint									m_iLevelNum = {};

private:
	// 특정 레벨의 특정 태그를 가진 레이어를 검색하는 함수
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelNum);
	virtual void Free() override;
};

NS_END