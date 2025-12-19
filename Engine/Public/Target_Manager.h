#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

	class CRenderTarget* Find_RenderTarget(const _wstring strTargetTag);
	class list<class CRenderTarget*>* Find_MRT(const _wstring strMRTTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	ID3D11Device*										m_pDevice = { nullptr };
	ID3D11DeviceContext*								m_pContext = { nullptr };
	map<const _wstring, class CRenderTarget*>			m_mapRenderTargets;
	map<const _wstring, list<class CRenderTarget*>>		m_mapMRTs;

	ID3D11RenderTargetView* m_pBackBuffer = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };


public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END