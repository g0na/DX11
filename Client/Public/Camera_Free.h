#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		CTransform* pPlayerTransform;
		_float		fSensor;
	}CAMERA_FREE_DESC;

protected:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual void Update_Priority(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Update_Late(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTransform*		m_pPlayerTransform = { nullptr };

	_float			m_fSensor = {};
	_float			m_fDistance = {};
	_float			m_fYaw = {};
	_float			m_fPitch = {};

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END