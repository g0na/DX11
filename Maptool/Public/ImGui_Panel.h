#pragma once
#include "Base.h"

NS_BEGIN(Maptool)

class CImGui_Panel abstract : public CBase
{
protected:
	explicit CImGui_Panel(const _char * pLabel);
	virtual ~CImGui_Panel() = default;
public:
	virtual HRESULT Intiailzie() PURE;
	virtual void Render() PURE;
protected:
	string m_strLabel = { "" };
public:
	virtual void Free() override;
};

NS_END