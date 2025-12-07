#pragma once
#include "GameObject.h"

// PartObject 여러 개를 들고있기 위한 기능

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract: public CGameObject
{
protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;
	
public:
	list<CGameObject*>	Get_PartObjects() { return m_listPartObjects; }

public:
	virtual HRESULT Initialize_Prototype() override;		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	map<const _wstring, class CPartObject*>		m_mapPartObjects;
	list<CGameObject*>							m_listPartObjects;

protected:
	HRESULT Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjTag, void* pArg);

protected:
	class CPartObject* Find_PartObject(const _wstring& strPartTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END