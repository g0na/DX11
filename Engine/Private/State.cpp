#include "State.h"

CState::CState()
{
}

HRESULT CState::Initialize(CGameObject* pOwner)
{
	if (pOwner == nullptr)
		return E_FAIL;

	m_pOwner = pOwner;

	return S_OK;
}

void CState::Free()
{
	__super::Free();
}
