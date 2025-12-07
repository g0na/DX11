#include "Collision_Manager.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Check_Collision(list<CGameObject*> pDstList, list<CGameObject*> pSrcList)
{
	for (auto& Dst : pDstList)
	{
		if (Dst->Get_IsCollisionEnabled() == false)
			continue;

		for (auto& Src : pSrcList)
		{
			if (Src->Get_IsCollisionEnabled() == false)
				continue;

			// 구와 구 충돌
			if (Dst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_ColType() == COLLIDER::SPHERE &&
				Src->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_ColType() == COLLIDER::SPHERE)
			{
				// 구 충돌
				Collision_Sphere(Dst, Src);
			}
		}
	}
}

void CCollision_Manager::Collision_Sphere(CGameObject* pDst, CGameObject* pSrc)
{
	// 충돌 검사
	if (Check_Sphere(pDst, pSrc))
	{
		// 콜라이더가 겹쳤을 때 서로 충돌 중이지 않았다면 충돌 함수 호출. 충돌중인 오브젝트 리스트에 등록
		if (pDst->IsCollidingWith(pSrc) == false)
		{
			pDst->Add_CollisionList(pSrc);
			pDst->OnCollisionEnter(pSrc);

			pSrc->Add_CollisionList(pDst);
			pSrc->OnCollisionEnter(pDst);

		#ifdef _DEBUG
			pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(true);
			pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(true);
		#endif
		}
	}
	else
	{
		// 콜라이더가 겹치지 않았을 때 서로 충돌 중이었다면 충돌 종료 함수 호출. 충돌중인 오브젝트 리스트에서 삭제
		if (pDst->IsCollidingWith(pSrc) == true)
		{
			pDst->Remove_CollisionList(pSrc);
			pDst->OnCollisionExit(pSrc);

			pSrc->Remove_CollisionList(pSrc);
			pSrc->OnCollisionExit(pSrc);

		#ifdef _DEBUG
			pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(false);
			pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(false);
		#endif
		}
	}
}

_bool CCollision_Manager::Check_Sphere(CGameObject* pDst, CGameObject* pSrc)
{
	CBounding_Sphere* pDstSphere = dynamic_cast<CBounding_Sphere*>(pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_Bounding());
	CBounding_Sphere* pSrcSphere = dynamic_cast<CBounding_Sphere*>(pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_Bounding());
	if (pDstSphere == nullptr ||
		pSrcSphere == nullptr)
		return false;

	// 두 구의 반지름의 합
	_float fRadius = pDstSphere->Get_Desc()->Radius + pSrcSphere->Get_Desc()->Radius;

	// 두 구의 중점 사이의 거리
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pDstSphere->Get_Desc()->Center) - XMLoadFloat3(&pSrcSphere->Get_Desc()->Center)));

	// 반지름의 합이 중점 사이의 거리보다 크다면 충돌
	return fRadius > fDistance;
}

CCollision_Manager* CCollision_Manager::Create()
{
	return new CCollision_Manager();
}

void CCollision_Manager::Free()
{
	__super::Free();
}
