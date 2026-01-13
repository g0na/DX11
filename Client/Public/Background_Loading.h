#pragma once
#include "Client_Defines.h"
#include "UIObj.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBackground_Loading final : public CUIObj
{
private:
	explicit CBackground_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBackground_Loading(const CBackground_Loading& Prototype);		// 메모리 복사를 통해서 객체를 생성
	virtual ~CBackground_Loading() = default;

public:
	virtual HRESULT Initialize_Prototype();					// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg);					// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Update_Late(_float fTimeDelta);
	virtual HRESULT	Render();

private:
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };

private:
	// 아틀라스 이미지 8 x 4 = 32 이미지
	// 프레임당 0.1초
	const _uint m_iFrameCnt = 32;
	const _uint m_iCountX = 8;
	const _uint m_iCountY = 4;

	const _float m_fSizeX = { 1.f / (_float)m_iCountX };
	const _float m_fSizeY = { 1.f / (_float)m_iCountY };

	_float2 m_vOffsetUV = {};
	_float2 m_vScaleUV = {};

	_uint m_iCurrentFrameIdx = {};
	_float m_fFrameDelay = {};

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static	CBackground_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END