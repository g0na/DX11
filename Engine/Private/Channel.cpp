#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, CModel* pModel)
{
    m_iBoneIndex = pModel->Get_BoneIndex(pAIChannel->mNodeName.data);
    if (m_iBoneIndex == -1)
        return E_FAIL;

    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3     vScale{};
    _float4     vRotation{};
    _float3     vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME    KeyFrame{};

        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;
        
        m_vecKeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

void CChannel::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition)
{
    // fCurrentTrackPosition에 맞는 현재 뼈의 상태를 만든다.
    _float4x4       TransformationMatrix = {};


    vecBones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();

    m_vecKeyFrames.clear();
}