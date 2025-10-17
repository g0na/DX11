#pragma once
#include "Base.h"
#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		// 객체에게 공통적으로 필요한 요소
		_tchar	szName[MAX_PATH] = {};
	}GAMEOBJECT_DESC;
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGameObject(const CGameObject& prototype);		// 메모리 복사를 통해 객체를 생성
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg);							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Update_Late(_float fTimeDelta);
	virtual HRESULT	Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	_tchar								m_szName[MAX_PATH] = {};
	map<const _wstring, CComponent*>	m_mapComponents;
	CTransform*					m_pTransformCom = { nullptr };

protected:
	/// <summary>
	/// 복제된 원본 컴포넌트를 생성하여 추가하는 함수
	/// </summary>
	/// <param name="iPrototypeLevelID">복제본이 위치한 레벨 ID</param>
	/// <param name="strPrototypeTag">컴포넌트 복제본의 태그</param>
	/// <param name="strComponentTag">컴포넌트 원본 태그</param>
	/// <param name="ppOut">결과로 내보낼 주소값</param>
	/// <param name="pArg"></param>
	/// <returns></returns>
	HRESULT Add_Component(_uint iPrototypeLevelID, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END