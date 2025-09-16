#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;
public:
	virtual _uint	AddRef();
	virtual _uint	Release();

public:
	virtual void Free();

private:
	_uint	m_iRefCnt = {};
};

NS_END