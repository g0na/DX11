#pragma once
#include "Base.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	void	Check_Collision(list<CGameObject*> pDstList, list<CGameObject*> pSrcList);
	void	Collision_Sphere(CGameObject* pDst, CGameObject* pSrc);
	_bool	Check_Sphere(CGameObject* pDst, CGameObject* pSrc);
	_bool	Check_Sphere_Bounding(class CBounding_Sphere* pDst, class  CBounding_Sphere* pSrc);
	_bool	Check_Sphere_Multi(class CCollider* pDst, const vector<class CCollider*>* pSrcList);

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END