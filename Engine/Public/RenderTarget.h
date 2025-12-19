#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void Clear();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	_float4x4		m_WorldMatrix;
#endif // _DEBUG

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	ID3D11Texture2D*				m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*			m_pRTV = { nullptr };				// 쓰기뷰, 픽셀 셰이더 출력이 저장되는 곳
	ID3D11ShaderResourceView*		m_pSRV = { nullptr };				// 읽기뷰, 셰이더에서 읽는 입력 텍스처, 버퍼

	_float4							m_vClearColor = {};

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

NS_END