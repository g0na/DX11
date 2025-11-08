#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Maptool)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager);

	enum class PanelType : unsigned int
	{
		INSPECTOR = 0,
		HIERARCHY,
		END
	};
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Render();

private:
	HRESULT Ready_Panels();
	void Render_Panels();

private:
	void Picking_GameObject();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CCalculator* m_pCalculator = { nullptr };

	ImGuiIO* m_pIO = { nullptr };
	class CImGui_Panel* m_pPanels[ENUM_TO_UINT(PanelType::END)] = { nullptr };

public:
	virtual void Free() override;
};

NS_END