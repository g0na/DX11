#pragma once
#include "Base.h"
#include "UIObj.h"

NS_BEGIN(Engine)

class CUI_Manager final : public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT		Add_UI(_wstring strUITag, CUIObj* pUIObj);
	CUIObj*		Find_UI(_wstring strUITag);
	
	void		Show_UI(_wstring strUITag);
	void		Hide_UI(_wstring strUITag);

private:
	map<_wstring, CUIObj*>		m_mapUIs;

public:
	static CUI_Manager* Create();
	virtual void Free() override;
};

NS_END