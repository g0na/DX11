#pragma once
#include "Base.h"

// 특정 동작(공격, 대기, 걷기 등등)을 위한 뼈대들의 시간에 따른 상태값(행렬)을 보관한다.
// CChannel : 특정 뼈의 시간에 따른 상태값(행렬)을 보관

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ofstream& file, const aiAnimation* pAIAnimation, class CModel* pModel);
	HRESULT Initialize_Binary(ifstream& file);
	_bool Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop);
	void Reset_TrackPosition();

public:
	HRESULT Write_To_Binary(ofstream& file);
	HRESULT Read_From_Binary(ifstream& file);

private:
	// 현재 동작을 구현해주기위해 사용해야하는 뼈의 개수
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_vecChannels;
	vector<_uint>				m_CurrentKeyFrameIndices = {};

	_char						m_szName[MAX_PATH] = {};
	_float						m_fTickPerSecond = {};				// 애니메이션의 재생 속도
	_float						m_fDuration = {};					// 현재 애니메이션의 전체 재생 길이
	_float						m_fCurrentTrackPosition = {};

public:
	static CAnimation* Create(ofstream& file, const aiAnimation* pAIAnimation, class CModel* pModel);
	static CAnimation* Create_Binary(ifstream& file);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END