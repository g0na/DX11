#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, CModel* pModel)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);
    m_fDuration = pAIAnimation->mDuration;
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

    // 현재 애니메이션을 위해 조절해야하는 뼈의 개수
    m_iNumChannels = pAIAnimation->mNumChannels;

    // 각 뼈가 시간에 따라 어떻게 움직일지에 대한 정보를 채널 객체에 저장한다.
    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (pChannel == nullptr)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {

    }

    for (auto& pChannel : m_vecChannels)
    {
        pChannel->Update_TransformationMatrices(vecBones, m_fCurrentTrackPosition);
    }
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, CModel* pModel)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_vecChannels)
        Safe_Release(pChannel);
    m_vecChannels.clear();
}
