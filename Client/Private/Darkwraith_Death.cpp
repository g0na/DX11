#include "Darkwraith_Death.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CDarkwraith_Death::CDarkwraith_Death()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Death::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterModel = pOwner->Get_Component<CModel>(TEXT("Com_Model"));
    if (m_pMonsterModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CDarkwraith_Death::Enter_State()
{
    m_pGameInstance->PlaySoundW(TEXT("Player_Kill.wav"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
    m_pGameInstance->PlaySoundW(TEXT("Darkwraith_dead.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    static_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(3, false);
}

void CDarkwraith_Death::Update_State(_float fTimeDelta)
{
    m_fDeathTime += fTimeDelta;

    if (m_fDeathTime >= 2.63f)
    {
        static_cast<CMonster_Darkwraith*>(m_pOwner)->Set_CollisionEnabled(false);
        //static_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Activity(false);
    }

}

void CDarkwraith_Death::Exit_State()
{
}

CDarkwraith_Death* CDarkwraith_Death::Create(CGameObject* pOwner)
{
    CDarkwraith_Death* pInstance = new CDarkwraith_Death();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Death::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
