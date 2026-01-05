#pragma once
#include "Client_Defines.h"
#include "UIObj.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Boss_HP final : public CUIObj
{
private:
	explicit CUI_Boss_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Boss_HP(const CUI_Boss_HP& Prototype);		// 메모리 복사를 통해서 객체를 생성
	virtual ~CUI_Boss_HP() = default;

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
	class CMonster_Boss*	m_pBoss = { nullptr };

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static	CUI_Boss_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END