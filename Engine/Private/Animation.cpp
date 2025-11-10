#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_iNumChannels { Prototype.m_iNumChannels }
    , m_vecChannels { Prototype.m_vecChannels }
    , m_fTickPerSecond { Prototype.m_fTickPerSecond }
    , m_fDuration { Prototype.m_fDuration }
    , m_fCurrentTrackPosition { Prototype.m_fCurrentTrackPosition }
    , m_CurrentKeyFrameIndices { Prototype.m_CurrentKeyFrameIndices }
{
    strcpy_s(m_szName, Prototype.m_szName);

    for (auto& pChannel : m_vecChannels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, CModel* pModel)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);
    m_fDuration = (_float)pAIAnimation->mDuration;
    m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

    // 현재 애니메이션을 위해 조절해야하는 뼈의 개수
    m_iNumChannels = pAIAnimation->mNumChannels;

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

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

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (isLoop == false)
            return true;

        m_fCurrentTrackPosition = 0.f;
    }

    _uint iIndex = {};

    for (auto& pChannel : m_vecChannels)
    {
        pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
    }

    return false;
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

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_vecChannels)
        Safe_Release(pChannel);
    m_vecChannels.clear();
}
