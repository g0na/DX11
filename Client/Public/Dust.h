#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CDust final : public CGameObject
{
private:
	CDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDust(const CDust& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~CDust() = default;

public:
	_bool	Get_IsOn() { return m_bIsOn; }

public:
	virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual void Update_Priority(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Update_Late(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Play(_fvector vResetPosition);

private:
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

private:
	_bool		m_bIsOn = {};

	const _uint m_iFrameCnt = 56;
	const _uint m_iCountX = 8;
	const _uint m_iCountY = 7;

	const _float m_fSizeX = { 1.f / (_float)m_iCountX };
	const _float m_fSizeY = { 1.f / (_float)m_iCountY };

	_float2 m_vOffsetUV = {};
	_float2 m_vScaleUV = {};

	_uint m_iCurrentFrameIdx = {};
	_float m_fFrameDelay = {};
	_float m_fTransparency = { 1.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END