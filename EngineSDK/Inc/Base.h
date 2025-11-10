#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	void		Set_Prototype(PROTOTYPE ePrototype) { m_ePrototype = ePrototype; }
	PROTOTYPE	Get_Prototype() { return m_ePrototype; }
public:
	virtual _uint	AddRef();
	virtual _uint	Release();

public:
	virtual void Free();

private:
	_uint	m_iRefCnt = {};
	PROTOTYPE	m_ePrototype = {};
};

NS_END