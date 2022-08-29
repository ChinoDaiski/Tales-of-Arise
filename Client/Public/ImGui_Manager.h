#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CImGUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGUI_Manager)
private:
	CImGUI_Manager();
	virtual ~CImGUI_Manager() = default;
public:
	HRESULT Init_ImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT StartFrame(void);
	HRESULT Set_Contents(void);
public:
	HRESULT Render(void);
public:
	void InitWin32(HWND hWnd);
	LRESULT WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	void OnOffImgui(void) { m_bImguiEnable = !m_bImguiEnable; }
	bool isImguiEnable() { return m_bImguiEnable; }
public:
	void Set_FireAvatar(class CFire_Avatar* pObject) {
		m_pFire_Avatar = pObject;
	}
private:
	void Setting_PostProcessing();
	void Setting_FireAvatar();
private:
	bool m_bImguiEnable = true;
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
private:
	class CFire_Avatar* m_pFire_Avatar = nullptr;
public:
	virtual void Free() override;
};

END