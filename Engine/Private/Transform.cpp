#include "Transform.h"
#include "UIObj.h"

USING(Engine)

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	// D3DXMatrixIdentity(&결과, &행렬)
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += vPosition * XMVector3Normalize(vLook) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= vPosition * XMVector3Normalize(vLook) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += vPosition * XMVector3Normalize(vRight) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= vPosition * XMVector3Normalize(vRight) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// 기존 Scale 값을 받아온다.
	_float3 vOriginScale = Get_Scale();

	// 3개의 축을 항등 벡터로 만들고 기존 Scale 값을 곱하여 Scale을 유지한다.
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vOriginScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vOriginScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vOriginScale.z;
	
	// 임의의 축으로 회전하는 행렬을 구한다.
	_matrix matRotation = XMMatrixRotationAxis(vAxis, fRadian);

	// 각 축을 회전 행렬과 곱하여 회전 시킨다.
	vRight = XMVector3TransformNormal(vRight, matRotation);
	vUp = XMVector3TransformNormal(vUp, matRotation);
	vLook = XMVector3TransformNormal(vLook, matRotation);

	// 회전한 결과 벡터를 다시 행렬에 설정해준다.
	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
	// 기존 Scale 값을 받아온다.
	_float3 vOriginScale = Get_Scale();

	// 3개의 축을 항등 벡터로 만들고 기존 Scale 값을 곱하여 Scale을 유지한다.
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vOriginScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vOriginScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vOriginScale.z;

	// 쿼터니언으로 회전하는 행렬을 구한다.
	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
	_matrix matQuaternion = XMMatrixRotationQuaternion(vQuaternion);

	// 각 축을 회전 행렬과 곱하여 회전 시킨다.
	vRight = XMVector3TransformNormal(vRight, matQuaternion);
	vUp = XMVector3TransformNormal(vUp, matQuaternion);
	vLook = XMVector3TransformNormal(vLook, matQuaternion);

	// 회전한 결과 벡터를 다시 행렬에 적용시킨다.
	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Turn(_fvector fAxis, _float fTimeDelta)
{
	// 기존 3개의 축을 가져온다
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);

	// 임의의 축으로 회전하는 행렬을 구한다.
	_matrix matRotation = XMMatrixRotationAxis(fAxis, m_fRotationPerSec * fTimeDelta);

	// 각 축을 회전 행렬과 곱하여 회전 시킨다.
	vRight = XMVector3TransformNormal(vRight, matRotation);
	vUp = XMVector3TransformNormal(vUp, matRotation);
	vLook = XMVector3TransformNormal(vLook, matRotation);

	// 회전한 결과 벡터를 다시 행렬에 적용시킨다.
	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::LookAt(_vector vTargetPos)
{
	// 기존의 Scale 값을 가져온다.
	_float3 vScale = Get_Scale();

	// 바라볼 위치 벡터에서 자신의 위치 벡터를 빼서 결과적인 Look 벡터를 만든다.
	_vector vLook = vTargetPos - Get_State(STATE::POSITION);
	
	// (0, 1, 0)과 vLook을 외적해서 Right 벡터를 얻는다.
	_vector vRight = XMVector3Cross(_vector{ 0.f, 1.f, 0.f }, vLook);

	// Look 벡터와 Right 벡터를 외적해서 Up 벡터를 얻는다.
	_vector vUp = XMVector3Cross(vLook, vRight);	

	// 목표 지점을 바라보는 3개의 벡터를 정규화하고 기존 Scale 값을 곱하여 월드 행렬에 적용한다.
	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Chase(_vector vTargetPos, _float fTimeDelta, _float fMinDist)
{
	// 자신의 위치벡터에서 타겟 위치벡터를 빼서 움직여야할 방향을 구한다.
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vMoveDir = vTargetPos - vPosition;

	// 매개변수로 들어오는 최소 거리보다 더 멀다면 그 방향으로 전진한다.
	if (XMVectorGetX(XMVector3Length(vMoveDir)) >= fMinDist)
		vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();


}