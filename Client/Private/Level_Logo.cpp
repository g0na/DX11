#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Background.h"

USING(Client);

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel {pDevice, pContext}
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_Background(TEXT("Layer_Background"))))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_pGameInstance->PlaySoundW(TEXT("GAMESTART.wav"), CHANNELID::SOUND_EFFECT, 1.f);

		if (FAILED(m_pGameInstance->Change_Level(ENUM_TO_UINT(LEVELID::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("·Î°í·¹º§ÀÔ´Ï´Ù."));
#endif

	m_pGameInstance->Draw_Text(TEXT("Font_English"), TEXT("Game Start"), _float2(g_iWinSizeX * 0.5f - 100.f, g_iWinSizeY * 0.75f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Fonts()
{
	/*MakeSpriteFont "³Ø½¼lv1°íµñ Bold" /FontSize:16 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 156ex.spritefont */

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_English"), TEXT("../Bin/Resources/Fonts/English.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Background(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Background"), 
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_GameObject_Background"),
		ENUM_TO_UINT(LEVELID::LOGO), strLayerTag)))))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
