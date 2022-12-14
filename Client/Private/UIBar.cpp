#include "stdafx.h"
#include "UIBar.h"
#include"UI_Manager.h"
#include"Player_Manager.h"
#include"Player.h"
#include"Alphen.h"
#include"Shionne.h"
//전투
UIBar::UIBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUI(pDevice, pDeviceContext)
{
}

UIBar::UIBar(const UIBar& rhs)
	: CUI(rhs)
{
}

HRESULT UIBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UIBar::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		return E_FAIL;
	}

	//인덱스 넣어주자.
	m_PlayerIndex = ((UIbarDesc*)pArg)->playerindex;

	if (FAILED(SetUp_Texture()))
	{
		return E_FAIL;
	}

	m_fDistanceToCamera = 40;
	m_iShaderPass = 22;
	CUI_Manager* pUiManager = CUI_Manager::GetInstance();
	Safe_AddRef(pUiManager);


	if (m_PlayerIndex == 0)
	{
		CPlayer* player = pUiManager->GetPlayer(0);
	}
	else if (m_PlayerIndex == 1)
	{
		CPlayer* player = pUiManager->GetPlayer(1);

	}
	Safe_Release(pUiManager);


	return S_OK;
}

void UIBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	CUI_Manager* pUiManager = CUI_Manager::GetInstance();
	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();

	if (GetAsyncKeyState(VK_NUMPAD7))
	{
		Set_Dead(1);
	}
	Safe_AddRef(pUiManager);
	Safe_AddRef(pPlayerManager);

	if (pPlayerManager->Get_MainPlayerIndex() != m_PlayerIndex && m_PlayerIndex == 0)
	{

		CPlayer* player = pUiManager->GetPlayer(0);
		if (player == nullptr) return;
		hp = 0;
		hp = dynamic_cast<CAlphen*>(player)->Get_PlayerInfo().m_iCurrentHp;
		percent = hp / dynamic_cast<CAlphen*>(player)->Get_PlayerInfo().m_iMaxHp;

	}
	else if (pPlayerManager->Get_MainPlayerIndex() != m_PlayerIndex  && m_PlayerIndex == 1)
	{
		CPlayer* player = pUiManager->GetPlayer(1);
		if (player == nullptr) return;
		hp = 0;
		hp = dynamic_cast<CShionne*>(player)->Get_PlayerInfo().m_iCurrentHp;
		percent = hp / dynamic_cast<CShionne*>(player)->Get_PlayerInfo().m_iMaxHp;

	}





	if (m_Redbar > percent)
	{
		m_Redbar -= TimeDelta;
	}
	if (m_Redbar < percent)
		m_Redbar = percent;

	Safe_Release(pUiManager);
	Safe_Release(pPlayerManager);

}

void UIBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT UIBar::Render()
{
	m_pShaderCom->Set_RawValue("lefthpPercent", &percent, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_leftRedBar", &m_Redbar, sizeof(_float));

	//if (FAILED(m_pShaderCom->Set_RawValue("exuirgb", &rgb, sizeof(float))))
	//	return E_FAIL;
	if (FAILED(__super::Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT UIBar::SetUp_Texture()
{
	//Prototype_Component_Texture_HPBar

	if (FAILED(__super::SetUp_Components(L"Com_Texture", LEVEL_STATIC, L"Prototype_Component_Texture_HPBar", (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX(L"Failed To UIBar : SetUp_Texture : SetUp_Component(Com_Texture)");
		return E_FAIL;
	}
	return S_OK;
}

UIBar * UIBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	UIBar* pInstance = new UIBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To UIBar : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * UIBar::Clone(void * pArg)
{
	UIBar* pInstance = new UIBar(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To UIBar : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void UIBar::Free()
{
	__super::Free();

}
