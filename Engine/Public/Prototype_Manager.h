#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	explicit CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iLevelNum);
	HRESULT Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg = nullptr);

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