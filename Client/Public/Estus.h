#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CEstus final : public CPartObject
{
public:
	typedef struct tagEstusDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}ESTUS_DESC;

private:
	CEstus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEstus(const CEstus& Prototype);
	virtual ~CEstus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_uint m_iRemain = {};		// 남은 에스트 개수

public:
	static CEstus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END