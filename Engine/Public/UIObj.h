#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObj abstract : public CGameObject
{
public:
	typedef struct tagUIObj final : public CGameObject::GAMEOBJECT_DESC
	{
		_float			fX{}, fY{}, fSizeX{}, fSizeY{};
	}UIOBJ_DESC;

protected:
	explicit CUIObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUIObj(const CUIObj& Prototype);	// 메모리 복사를 통해 객체를 생성
	virtual ~CUIObj() = default;

public:
	virtual HRESULT Initialize_Prototype();		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg);							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Update_Late(_float fTimeDelta);
	virtual HRESULT	Render();

protected:
	// 직교 투영을 위한 데이터들
	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4		m_ViewMatrix{}, m_ProjMatrix{};

protected:
	// 뷰, 투영행렬을 렌더링할 때 이용할 수 있도록 세팅해주는 작업
	HRESULT		Bind_OrthoMatrices(class CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END