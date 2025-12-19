#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	void Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLight*> m_listLights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

NS_END