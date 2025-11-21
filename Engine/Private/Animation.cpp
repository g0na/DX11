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

HRESULT CAnimation::Initialize(ofstream& file, const aiAnimation* pAIAnimation, CModel* pModel)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);
    m_fDuration = (_float)pAIAnimation->mDuration;
    m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

    // 현재 애니메이션을 위해 조절해야하는 뼈의 개수
    m_iNumChannels = pAIAnimation->mNumChannels;

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    Write_To_Binary(file);

    // 각 뼈가 시간에 따라 어떻게 움직일지에 대한 정보를 채널 객체에 저장한다.
    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (pChannel == nullptr)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    for (auto& pChannel : m_vecChannels)
        pChannel->Write_To_Binary(file);

    return S_OK;
}

HRESULT CAnimation::Initialize_Binary(ifstream& file)
{
    Read_From_Binary(file);

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);
    m_vecChannels.reserve(m_iNumChannels);

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create_Binary(file);
        if (pChannel == nullptr)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * 2.f;

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

void CAnimation::Reset_TrackPosition()
{
    m_fCurrentTrackPosition = 0.f;

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        m_CurrentKeyFrameIndices[i] = 0;
    }
}

HRESULT CAnimation::Write_To_Binary(ofstream& file)
{
    _uint iNameLength = (_uint)strlen(m_szName);
    file.write(CHARCAST(&iNameLength), sizeof(_uint));
    file.write(m_szName, iNameLength);
    file.write(CHARCAST(&m_fDuration), sizeof(_float));
    file.write(CHARCAST(&m_fTickPerSecond), sizeof(_float));
    file.write(CHARCAST(&m_iNumChannels), sizeof(_uint));

    return S_OK;
}

HRESULT CAnimation::Read_From_Binary(ifstream& file)
{
    _uint iNameLength;
    file.read(CHARCAST(&iNameLength), sizeof(_uint));
    file.read(m_szName, iNameLength);
    file.read(CHARCAST(&m_fDuration), sizeof(_float));
    file.read(CHARCAST(&m_fTickPerSecond), sizeof(_float));
    file.read(CHARCAST(&m_iNumChannels), sizeof(_uint));

    return S_OK;
}

CAnimation* CAnimation::Create(ofstream& file, const aiAnimation* pAIAnimation, CModel* pModel)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(file, pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Create_Binary(ifstream& file)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize_Binary(file)))
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
