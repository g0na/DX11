#include "Player_Death.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CPlayer_Death::CPlayer_Death()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Death::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    Safe_AddRef(m_pPlayerBody);
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));
    Safe_AddRef(m_pPlayerCamera);

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_Death::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(16, false);

    static_cast<CPlayer*>(m_pOwner)->Set_CollisionEnabled(false);
}

void CPlayer_Death::Update_State(_float fTimeDelta)
{
}

void CPlayer_Death::Exit_State()
{
}

CPlayer_Death* CPlayer_Death::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Death* pInstance = new CPlayer_Death();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Death::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerBody);
    Safe_Release(m_pPlayerCamera);
    Safe_Release(m_pGameInstance);
}
