#include "Background.h"

USING(Client)

CBackground::CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CBackground::CBackground(const CBackground& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CBackground::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackground::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	//CUIObj::UIOBJECT_DESC        Desc{};

	//Desc.fX = {};
	//Desc.fY = {};
	//Desc.fSizeX = {};
	//Desc.fSizeY = {};
	//lstrcpy(Desc.szName, TEXT("BackGround"));
	//Desc.fSpeedPerSec = 10.f;
	//Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBackground::Update_Priority(_float fTimeDelta)
{
	int a = 10;
}

void CBackground::Update(_float fTimeDelta)
{
	int a = 10;
}

void CBackground::Update_Late(_float fTimeDelta)
{
	int a = 10;
}

HRESULT CBackground::Render()
{
	return S_OK;
}

CBackground* CBackground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackground* pInstance = new CBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackground::Clone(void* pArg)
{
	CBackground* pInstance = new CBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CBackground::Free()
{
	__super::Free();

}
