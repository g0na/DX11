#pragma once
#include "Base.h"
#include "Engine_Define.h"

NS_BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	explicit CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Update();
	void Clear();

public:
	//// 3d 사운드 로드 함수
	//HRESULT LoadSound3D(const _wstring& pSoundKey, const _tchar* pFilePath);

	//// 3d 사운드를 특정 위치에서 재생하는 함수
	//void PlaySound3D(const _wstring& pSoundKey, const _float3& vPos, _float fVolume);

	// 리스너의 위치와 방향 설정해주는 함수
	//void SetListener(const _float3& vPos, const _float3& vForward, const _float3& vUp);
	void PlaySoundW(const _wstring& soundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const _wstring& soundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile();

private:
	// 사운드 폴더의 최종 경로를 갖는 변수
	wstring m_strSoundFolderPath;

	// 사운드 리소스 정보를 갖는 객체 
	map<_wstring, FMOD_SOUND*> m_mapSound2D;
	//map<_wstring, FMOD_SOUND*> m_mapSound3D;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[ENUM_TO_UINT(CHANNELID::MAXCHANNEL)];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	// 3D 사운드를 저장하는 컨테이너
	//list<FMOD_CHANNEL*> m_list3DSound;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

NS_END