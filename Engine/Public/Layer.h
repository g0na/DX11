#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pObj);
	HRESULT Delete_GameObject(class CGameObject* pObj);
	void	Update_Priority(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Update_Late(_float fTimeDelta);

public:
	list<class CGameObject*> Get_Objects() { return m_listObject; }

private:
	list<class CGameObject*>	m_listObject;

public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END