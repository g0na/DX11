#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
    : m_pSystem(nullptr)
{
    for (int i = 0; i < ENUM_TO_UINT(CHANNELID::MAXCHANNEL); ++i)
        m_pChannelArr[i] = nullptr;
}

HRESULT CSound_Manager::Initialize()
{
    FMOD_System_Create(&m_pSystem, FMOD_VERSION);
    FMOD_System_Init(m_pSystem, 128, FMOD_INIT_NORMAL, NULL);
    LoadSoundFile();

    //LoadSound3D(TEXT("Sound_Test"), TEXT("../../Client/Bin/Resources/Sounds/BOSSWALK.wav"));

    return S_OK;
}

void CSound_Manager::Update()
{
    FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Clear()
{
}

//HRESULT CSound_Manager::LoadSound3D(const _wstring& pSoundKey, const _tchar* pFilePath)
//{
//    auto iter = m_mapSound3D.find(pSoundKey);
//    if (iter != m_mapSound3D.end())
//        return S_OK;
//
//    char szPath[MAX_PATH];
//    size_t iConvertChars = 0;
//
//    wcstombs_s(&iConvertChars, szPath, MAX_PATH, pFilePath, _TRUNCATE);
//
//    FMOD_SOUND* pSound = nullptr;
//
//    if (FMOD_OK != FMOD_System_CreateSound(m_pSystem, szPath, FMOD_3D_INVERSEROLLOFF, NULL, &pSound))
//        return E_FAIL;
//    
//    FMOD_Sound_Set3DMinMaxDistance(pSound, 5.f, 20.f);
//
//    m_mapSound3D.emplace(pSoundKey, pSound);
//
//    return S_OK;
//}

//void CSound_Manager::PlaySound3D(const _wstring& pSoundKey, const _float3& vPos, _float fVolume)
//{
//    auto iter = m_mapSound3D.find(pSoundKey);
//    if (iter == m_mapSound3D.end())
//        return;
//
//    FMOD_CHANNEL* pChannel = nullptr;
//    FMOD_System_PlaySound(m_pSystem, iter->second, 0, true, &pChannel);
//
//    if (pChannel)
//    {
//        FMOD_VECTOR soundPos = { vPos.x ,vPos.y, vPos.z };
//        FMOD_VECTOR soundVel = { 0.f, 0.f, 0.f };
//        FMOD_Channel_Set3DAttributes(pChannel, &soundPos, &soundVel);
//        FMOD_Channel_SetVolume(pChannel, fVolume);
//        FMOD_Channel_SetPaused(pChannel, false);
//    }
//}

//void CSound_Manager::SetListener(const _float3& vPos, const _float3& vForward, const _float3& vUp)
//{
//    FMOD_VECTOR listenerPos = { vPos.x, vPos.y, vPos.z };
//    FMOD_VECTOR listenerForward = { vForward.x, vForward.y, vForward.z };
//    FMOD_VECTOR listenerUp = { vUp.x, vUp.y, vUp.z };
//    FMOD_VECTOR listenerVel = { 0.f, 0.f, 0.f };
//
//    FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
//}

void CSound_Manager::PlaySoundW(const _wstring& soundKey, CHANNELID eID, float fVolume)
{
    auto iter = m_mapSound2D.find(soundKey);
    if (iter == m_mapSound2D.end())
        return;

    FMOD_Channel_Stop(m_pChannelArr[ENUM_TO_UINT(eID)]);
    FMOD_System_PlaySound(
        m_pSystem,
        iter->second,
        0,
        FALSE,
        &m_pChannelArr[ENUM_TO_UINT(eID)]
    );

    FMOD_Channel_SetVolume(m_pChannelArr[ENUM_TO_UINT(eID)], fVolume);
}

void CSound_Manager::PlayBGM(const _wstring& soundKey, float fVolume)
{
    auto iter = m_mapSound2D.find(soundKey);
    if (iter == m_mapSound2D.end())
        return;

    FMOD_BOOL isPlaying = {};
    FMOD_Channel_IsPlaying(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)], &isPlaying);

    if (isPlaying)
    {
        if (m_strCurrentBGM == iter->first)
            return;
        else
        {
            FMOD_Channel_Stop(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)]);
            m_strCurrentBGM = iter->first;

            FMOD_System_PlaySound(
                m_pSystem,
                iter->second,
                0,
                FALSE,
                &m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)]
            );
            FMOD_Channel_SetMode(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)], FMOD_LOOP_NORMAL);
            FMOD_Channel_SetVolume(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)], fVolume);
        }
    }
    else
    {
        m_strCurrentBGM = iter->first;
        FMOD_System_PlaySound(
            m_pSystem,
            iter->second,
            0,
            FALSE,
            &m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)]
        );
        FMOD_Channel_SetMode(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)], FMOD_LOOP_NORMAL);
        FMOD_Channel_SetVolume(m_pChannelArr[ENUM_TO_UINT(CHANNELID::SOUND_BGM)], fVolume);
    }    
}

void CSound_Manager::StopSound(CHANNELID eID)
{
    FMOD_Channel_Stop(m_pChannelArr[ENUM_TO_UINT(eID)]);
}

void CSound_Manager::StopAll()
{
    for (int i = 0; i < ENUM_TO_UINT(CHANNELID::MAXCHANNEL); ++i)
        FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
    FMOD_Channel_SetVolume(m_pChannelArr[ENUM_TO_UINT(eID)], fVolume);
}

void CSound_Manager::LoadSoundFile()
{
    _finddata_t fd;
    intptr_t handle = _findfirst("../../Client/Bin/Resources/Sounds/*.*", &fd);
    if (handle == -1)
        return;

    do
    {
        // 디렉터리는 건너뛰기
        if (fd.attrib & _A_SUBDIR)
            continue;

        // ANSI fd.name → std::wstring 변환
        // fd.name → wname 변환 시
        int wlen = MultiByteToWideChar(CP_ACP, 0,
            fd.name, -1,
            nullptr, 0);
        // wlen 은 널 포함 길이, 예를 들어 "foo"면 4

        // 실제 담을 길이는 wlen-1
        std::wstring wname;
        wname.resize(wlen - 1);
        MultiByteToWideChar(CP_ACP, 0,
            fd.name, -1,
            &wname[0], wlen - 1);

        // 전체 경로 ANSI로 조합
        char szFullPath[256];
        sprintf_s(szFullPath, "../../Client/Bin/Resources/Sounds/%s", fd.name);

        // 사운드 로드
        FMOD_SOUND* pSound = nullptr;
        FMOD_RESULT res = FMOD_System_CreateSound(
            m_pSystem,
            szFullPath,
            FMOD_DEFAULT,
            0,
            &pSound
        );

        if (res == FMOD_OK)
        {
            // wname을 키로 바로 저장
            m_mapSound2D.emplace(wname, pSound);
        }

    } while (_findnext(handle, &fd) == 0);

    _findclose(handle);
}

CSound_Manager* CSound_Manager::Create()
{
    CSound_Manager* pInstance = new CSound_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create CSound_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_mapSound2D)
        FMOD_Sound_Release(Pair.second);
    m_mapSound2D.clear();

    //for (auto& Pair : m_mapSound3D)
    //    FMOD_Sound_Release(Pair.second);
    //m_mapSound3D.clear();

    FMOD_System_Close(m_pSystem);
    FMOD_System_Release(m_pSystem);
    m_pSystem = nullptr;
}
