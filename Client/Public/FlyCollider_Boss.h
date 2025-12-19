#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CFlyCollider_Boss final : public CPartObject
{
public:
	typedef struct tagFlyDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}FLYCOLLIDER_BOSS_DESC;

private:
	CFlyCollider_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlyCollider_Boss(const CFlyCollider_Boss& Prototype);
	virtual ~CFlyCollider_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOtherObject) override;
	virtual void OnCollisionExit(CGameObject* pOtherObject) override;

private:
	HRESULT Ready_Components();

private:
	CCollider*				m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

public:
	static CFlyCollider_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END