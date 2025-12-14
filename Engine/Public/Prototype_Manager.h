#pragma once
// CGameObject, CComponent들의 원본을 보관한다.
// 레벨별로 구분하여 보관한다. GameObject와 Component를 따로 구분하지는 않는다.

#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Navigation.h"

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	explicit CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	map<const _wstring, CBase*> const Get_Prototypes(_uint iLevelID) {
		return m_pPrototypes[iLevelID];
	}

public:
	HRESULT Initialize(_uint iLevelNum);
	HRESULT Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg = nullptr);
	void	Clear(_uint iLevelNum);

private:
	_uint							m_iLevelNum = {};
	map<const _wstring, CBase*>*	m_pPrototypes = { nullptr };

private:
	CBase* Find_Prototype(_uint iLevelNum, const _wstring& strProtoTag);

public:
	static CPrototype_Manager* Create(_uint iLevelNum);
	virtual void Free() override;
};

NS_END