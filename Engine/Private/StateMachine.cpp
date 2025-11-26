#include "StateMachine.h"

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

HRESULT CStateMachine::Add_State(_uint iStateType, CState* pState)
{
	return E_NOTIMPL;
}

HRESULT CStateMachine::Change_State(_uint iStateType)
{
	return E_NOTIMPL;
}

void CStateMachine::Set_State(_uint iStateType)
{
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = CStateMachine::Create(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	return nullptr;
}

void CStateMachine::Free()
{
}
