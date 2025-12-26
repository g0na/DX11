#include "Hollow_Death.h"
#include "Monster_Hollow.h"
#include "GameInstance.h"

CHollow_Death::CHollow_Death()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CHollow_Death::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    return S_OK;
}

void CHollow_Death::Enter_State()
{
    static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(3, false);
}

void CHollow_Death::Update_State(_float fTimeDelta)
{
    m_fDeathTime += fTimeDelta;

    if (m_fDeathTime >= 2.f)
    {
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_CollisionEnabled(false);
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_Activity(false);
    }

}

void CHollow_Death::Exit_State()
{
}

CHollow_Death* CHollow_Death::Create(CGameObject* pOwner)
{
    CHollow_Death* pInstance = new CHollow_Death();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CHollow_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHollow_Death::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
