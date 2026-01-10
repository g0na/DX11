#include "Boss_Death.h"
#include "Monster_Boss.h"
#include "GameInstance.h"

CBoss_Death::CBoss_Death()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Death::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    return S_OK;
}

void CBoss_Death::Enter_State()
{
    static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(3, false);
}

void CBoss_Death::Update_State(_float fTimeDelta)
{
    m_fDeathTime += fTimeDelta;

    if (m_fDeathTime >= 5.5f)
    {
        static_cast<CMonster_Boss*>(m_pOwner)->Set_CollisionEnabled(false);
        m_pGameInstance->Show_UI(TEXT("Prototype_UI_Victory_Back"));
        m_pGameInstance->Show_UI(TEXT("Prototype_UI_Victory"));
    }
    else if (m_fDeathTime >= 7.333f)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Activity(false);
}

void CBoss_Death::Exit_State()
{
}

CBoss_Death* CBoss_Death::Create(CGameObject* pOwner)
{
    CBoss_Death* pInstance = new CBoss_Death();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Death::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
