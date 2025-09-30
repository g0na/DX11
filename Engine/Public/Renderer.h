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
	void Draw_Blend();
	void Draw_UI();

private:
	list<class CGameObject*> m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::END)];

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END