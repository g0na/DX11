#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT	Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pObj);
	void Draw();

private:
	void Draw_Priority();
	void Draw_Nonblend();
	void Draw_Lights();
	void Draw_Combined();
	void Draw_NonLight();
	void Draw_Blend();
	void Draw_UI();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);

private:
	void Render_Debug();

private:
	list<class CComponent*>	m_listDebugComponents;
#endif // _DEBUG

private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	list<class CGameObject*>	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::END)];

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	class CShader*			m_pShader = { nullptr };

	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END