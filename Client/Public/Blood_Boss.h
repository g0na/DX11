#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBlood_Boss final : public CGameObject
{
public:
	
private:
	CBlood_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlood_Boss(const CBlood_Boss& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~CBlood_Boss() = default;

public:
	_bool Get_IsOn() { return m_bIsOn; }

public:
	virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual void Update_Priority(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Update_Late(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Play(_fvector vResetPosition, _fvector vTargetPosition);

private:
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

private:
	_bool		m_bIsOn = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBlood_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END