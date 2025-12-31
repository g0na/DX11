#include "Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CFont::Draw_Text(SpriteBatch* pBatch, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
	m_pFont->DrawString(pBatch, pText, vPosition, vColor);

	return S_OK;
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CFont* pInstance = new CFont(pDevice, pContext);

	/* 메인앱을 완벽히 사용하기 위한 기타 초기화작업을 수행한다. */
	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CFont::Free()
{
	__super::Free();

	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
