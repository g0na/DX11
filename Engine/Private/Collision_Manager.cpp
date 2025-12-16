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

			// 콜라이더 존재 여부 확인
			const vector<CCollider*>* pDstColliders = Dst->Get_Colliders();
			const vector<CCollider*>* pSrcColliders = Src->Get_Colliders();
			CCollider* pDstSingle = Dst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"));
			CCollider* pSrcSingle = Src->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"));

          // Dst 콜라이더 유효성 체크
			_bool bDstValid = false;
			if (pDstColliders != nullptr && pDstColliders->size() > 0)
				bDstValid = true;  // 여러 콜라이더 있음
			else if (pDstSingle != nullptr && pDstSingle->Is_Active())
				bDstValid = true;  // 단일 콜라이더 있고 활성화

          // Src도 동일하게 체크
			_bool bSrcValid = false;
			if (pSrcColliders != nullptr && pSrcColliders->size() > 0)
				bSrcValid = true;
			else if (pSrcSingle != nullptr && pSrcSingle->Is_Active())
				bSrcValid = true;

          // 둘 다 유효하면 충돌 검사
          if (bDstValid && bSrcValid)
              Collision_Sphere(Dst, Src);
		}
	}
}

void CCollision_Manager::Collision_Sphere(CGameObject* pDst, CGameObject* pSrc)
{
	_bool bCollision = { false };

	const vector<CCollider*>* pDstColliders = pDst->Get_Colliders();
	const vector<CCollider*>* pSrcColliders = pSrc->Get_Colliders();

	if (pDstColliders != nullptr && pDstColliders->size() > 0)
	{
		CCollider* pSrcCollider = pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"));

		if (pSrcCollider != nullptr)
			bCollision = Check_Sphere_Multi(pSrcCollider, pDstColliders);
	}
	else if (pSrcColliders != nullptr && pSrcColliders->size() > 0)
	{
		CCollider* pDstCollider = pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"));

		if (pDstCollider != nullptr)
			bCollision = Check_Sphere_Multi(pDstCollider, pSrcColliders);
	}
	else
	{
		bCollision = Check_Sphere(pDst, pSrc);
	}

	// 충돌 검사
	if (bCollision)
	{
		// 콜라이더가 겹쳤을 때 서로 충돌 중이지 않았다면 충돌 함수 호출. 충돌중인 오브젝트 리스트에 등록
		if (pDst->IsCollidingWith(pSrc) == false)
		{
			pDst->Add_CollisionList(pSrc);
			pDst->OnCollisionEnter(pSrc);

			pSrc->Add_CollisionList(pDst);
			pSrc->OnCollisionEnter(pDst);
		}
	}
	else
	{
		// 콜라이더가 겹치지 않았을 때 서로 충돌 중이었다면 충돌 종료 함수 호출. 충돌중인 오브젝트 리스트에서 삭제
		if (pDst->IsCollidingWith(pSrc) == true)
		{
			pDst->Remove_CollisionList(pSrc);
			pDst->OnCollisionExit(pSrc);

			pSrc->Remove_CollisionList(pDst);
			pSrc->OnCollisionExit(pDst);
		}
	}


#ifdef _DEBUG
	if (pDstColliders != nullptr && pDstColliders->size() > 0)
	{
		for (auto& pCollider : *pDstColliders)
			pCollider->Set_IsColl(pDst->Get_CollidingObjectsCnt() != 0);
	}
	else
	{
		pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(pDst->Get_CollidingObjectsCnt() != 0);
	}

	if (pSrcColliders != nullptr && pSrcColliders->size() > 0)
	{
		for (auto& pCollider : *pSrcColliders)
			pCollider->Set_IsColl(pDst->Get_CollidingObjectsCnt() != 0);
	}
	else
		pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Set_IsColl(pSrc->Get_CollidingObjectsCnt() != 0);
#endif
}

_bool CCollision_Manager::Check_Sphere(CGameObject* pDst, CGameObject* pSrc)
{
	CBounding_Sphere* pDstSphere = static_cast<CBounding_Sphere*>(pDst->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_Bounding());
	CBounding_Sphere* pSrcSphere = static_cast<CBounding_Sphere*>(pSrc->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_Bounding());
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

_bool CCollision_Manager::Check_Sphere_Bounding(CBounding_Sphere* pDst, CBounding_Sphere* pSrc)
{
	if (pDst == nullptr || pSrc == nullptr)
		return false;

	// 두 구의 반지름의 합
	_float fRadius = pDst->Get_Desc()->Radius + pSrc->Get_Desc()->Radius;

	// 두 구의 중점 사이의 거리
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pDst->Get_Desc()->Center) - XMLoadFloat3(&pSrc->Get_Desc()->Center)));

	// 반지름의 합이 중점 사이의 거리보다 크다면 충돌
	return fRadius > fDistance;
}

_bool CCollision_Manager::Check_Sphere_Multi(CCollider* pDst, const vector<CCollider*>* pSrcList)
{
	CBounding_Sphere* pDstSphere = static_cast<CBounding_Sphere*>(pDst->Get_Bounding());
	if (pDstSphere == nullptr)
		return false;

	for (auto& pCollider : *pSrcList)
	{
		if (pCollider == nullptr || pCollider->Is_Active() == false)
			continue;

		if (Check_Sphere_Bounding(pDstSphere, static_cast<CBounding_Sphere*>(pCollider->Get_Bounding())) == true)
			return true;
		else
			continue;
	}

	return false;
}

CCollision_Manager* CCollision_Manager::Create()
{
	return new CCollision_Manager();
}

void CCollision_Manager::Free()
{
	__super::Free();
}
