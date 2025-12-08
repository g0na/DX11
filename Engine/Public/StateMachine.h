#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CState;

class ENGINE_DLL CStateMachine final : public CComponent
{
private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& Prototype);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update_State(_float fTimeDelta);

public:
	_bool	Get_BoolData(_wstring strBoolTag, _bool bValue);
	_int	Get_IntData(_wstring strIntTag, _int iValue);
	_float	Get_FloatData(_wstring strFloatTag, _float fValue);
	_vector Get_VectorData(_wstring strVectorTag, _vector vecValue);

	void	Set_BoolData(_wstring strBoolTag, _bool bValue);
	void	Set_IntData(_wstring strIntTag, _int iValue);
	void	Set_FloatData(_wstring strFloatTag, _float fValue);
	void	Set_VectorData(_wstring strVectorTag, _vector vecValue);

public:
	// 상태 추가
	HRESULT Add_State(_uint iStateType, CState* pState);
	// 상태 전환
	HRESULT Change_State(_uint iStateType);
	// 초기 세팅용
	void Set_State(_uint iStateType);

private:
	map<_wstring, _bool>	m_mapBoolData;
	map<_wstring, _int>		m_mapIntData;
	map<_wstring, _float>	m_mapFloatData;
	map<_wstring, _vector>	m_mapVectorData;

private:
	map<_uint, CState*> m_mapStates;

	CState* m_pCurState = { nullptr };
	CState* m_pPrevState = { nullptr };

	_uint m_iCurStateType = {};
	_uint m_iPrevStateType = {};

public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END