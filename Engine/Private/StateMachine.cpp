#include "StateMachine.h"
#include "State.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CStateMachine::CStateMachine(const CStateMachine& Prototype)
	: CComponent { Prototype }
{
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	return S_OK;
}

void CStateMachine::Update_State(_float fTimeDelta)
{
	if (m_pCurState == nullptr)
		return;

	m_pCurState->Update_State(fTimeDelta);
}

HRESULT CStateMachine::Add_State(_uint iStateType, CState* pState)
{
	if (pState == nullptr)
		return E_FAIL;

	m_mapStates.emplace(iStateType, pState);

	return S_OK;
}

HRESULT CStateMachine::Change_State(_uint iStateType)
{
	// 같은 상태라면 무시
	if (iStateType == m_iCurStateType)
		return S_OK;
	
	// 새 상태 검사
	auto iter = m_mapStates.find(iStateType);
	if (iter == m_mapStates.end())
		return E_FAIL;

	// 검사 후 현재 상태를 이전 상태로 저장
	m_iPrevStateType = m_iCurStateType;
	m_pPrevState = m_pCurState;

	if (m_pCurState != nullptr)
		m_pCurState->Exit_State();

	// 새 상태 전환
	m_iCurStateType = iStateType;
	m_pCurState = iter->second;
	m_pCurState->Enter_State();

	return S_OK;
}

void CStateMachine::Set_State(_uint iStateType)
{
	// 처음 설정하는 상태 검사
	auto iter = m_mapStates.find(iStateType);
	if (iter == m_mapStates.end())
		return;

	// 상태 입장
	m_pCurState = iter->second;
	m_pCurState->Enter_State();
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();

	for (auto& Pair : m_mapStates)
		Safe_Release(Pair.second);
	m_mapStates.clear();
}
