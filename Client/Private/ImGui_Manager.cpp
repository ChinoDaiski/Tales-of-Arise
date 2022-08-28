#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "..\Public\ImGUI_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGUI_Manager)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CImGUI_Manager::CImGUI_Manager()
{
}

HRESULT CImGUI_Manager::Init_ImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	ImGui_ImplDX11_Init(*ppDeviceOut, *ppDeviceContextOut);

	m_pDevice = *ppDeviceOut;
	m_pDeviceContext = *ppDeviceContextOut;

	return S_OK;
}

HRESULT CImGUI_Manager::StartFrame(void)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return S_OK;
}

HRESULT CImGUI_Manager::Set_Contents(void)
{
	ImGui::Begin("PostProcessing");
	Setting_PostProcessing();
	ImGui::End();

	return S_OK;
}

HRESULT CImGUI_Manager::Render(void)
{
	if (m_bImguiEnable)
	{
		StartFrame();
		Set_Contents();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return S_OK;
}

void CImGUI_Manager::InitWin32(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
}

LRESULT CImGUI_Manager::WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void CImGUI_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void CImGUI_Manager::Setting_PostProcessing()
{
	CGameInstance* pGameIstance = GET_INSTANCE(CGameInstance);

	_float fMiddleGray = pGameIstance->Get_MiddleGray();
	if (ImGui::SliderFloat("MiddleGray", &fMiddleGray, 0.f, 5.f))
	{
		pGameIstance->Set_MiddleGray(fMiddleGray);
	}

	_float fWhite = pGameIstance->Get_White();
	if (ImGui::SliderFloat("White", &fWhite, 0.f, 2.f))
	{
		pGameIstance->Set_White(fWhite);
	}
	RELEASE_INSTANCE(CGameInstance);
}