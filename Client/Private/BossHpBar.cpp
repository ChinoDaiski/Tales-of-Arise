#include "stdafx.h"
#include "..\Public\BossHpBar.h"
#include "Player_Manager.h"
#include"Player.h"
BossHpBar::BossHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUI(pDevice, pDeviceContext)
{
}


BossHpBar::BossHpBar(const BossHpBar& rhs)
	:CUI(rhs)
{

}

HRESULT BossHpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT BossHpBar::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		return E_FAIL;
	}

	m_kindnum = ((MONSTERUIBARDESC*)pArg)->kind;

	if (FAILED(SetUp_Texture()))
	{
		return E_FAIL;
	}

	m_fDistanceToCamera = 40;
	m_iShaderPass = 26;
	return S_OK;
}

void BossHpBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();
	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();
	Safe_AddRef(pPlayerManager);

	CPlayer* pPlayer = pPlayerManager->Get_MainPlayer();


	//GetMaxHP 받아와서 /4 로 한다음에  4지점 갈때마다 바꾸자
	if (m_kindnum == 0 && m_monsterHPindex == 0)
	{
		if (m_percent <= 0)
		{
			m_isZero = true;
			return;
		}
		else
		{
			m_isZero = false;
		}
		m_percent = pPlayer->Get_TargetEnemyHP()/4 /( pPlayer->Get_TargetEnemyMaxHP()/4);

		if (m_Redbar > m_percent)
		{
			m_Redbar -= TimeDelta;
		}
		if (m_Redbar < m_percent)
			m_Redbar = m_percent;

	}

	if (m_kindnum == 1 && m_monsterHPindex == 1)
	{
		if (m_percent <= 0)
		{
			m_isZero = true;
		}
		else
		{
			m_isZero = false;
		}
		m_percent = pPlayer->Get_TargetEnemyHP() / pPlayer->Get_TargetEnemyMaxHP();

		if (m_Redbar > m_percent)
		{
			m_Redbar -= TimeDelta;
		}
		if (m_Redbar < m_percent)
			m_Redbar = m_percent;

	}

	if (m_kindnum == 2 && m_monsterHPindex == 2)
	{
		if (m_percent <= 0)
		{
			m_isZero = true;
		}
		else
		{
			m_isZero = false;
		}
		m_percent = pPlayer->Get_TargetEnemyHP() / pPlayer->Get_TargetEnemyMaxHP();

		if (m_Redbar > m_percent)
		{
			m_Redbar -= TimeDelta;
		}
		if (m_Redbar < m_percent)
			m_Redbar = m_percent;
		
		
	}
	if (m_kindnum == 3 && m_monsterHPindex == 3)
	{
		if (m_percent <= 0)
		{
			m_isZero = true;
		}
		else
		{
			m_isZero = false;
		}
		m_percent = pPlayer->Get_TargetEnemyHP() / pPlayer->Get_TargetEnemyMaxHP();


		if (m_Redbar > m_percent)
		{
			m_Redbar -= TimeDelta;
		}
		if (m_Redbar < m_percent)
			m_Redbar = m_percent;

	}

	Safe_Release(pPlayerManager);

}

void BossHpBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT BossHpBar::Render()
{
	m_pShaderCom->Set_RawValue("Monsterhoripercent", &m_percent, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_MonsterRedbar", &m_Redbar, sizeof(_float));
	if (FAILED(__super::Render()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT BossHpBar::SetUp_Texture()
{
	//Prototype_Component_Texture_HPBar

	if (FAILED(__super::SetUp_Components(L"Com_Texture", LEVEL_STATIC, L"Prototype_Component_Texture_HPBar", (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX(L"Failed To MonsterUIBar : SetUp_Texture : SetUp_Component(Com_Texture)");
		return E_FAIL;
	}
	return S_OK;
}

BossHpBar * BossHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BossHpBar* pInstance = new BossHpBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To MonsterUIBar : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * BossHpBar::Clone(void * pArg)
{
	BossHpBar* pInstance = new BossHpBar(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To MonsterUIBar : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void BossHpBar::Free()
{
	__super::Free();
}
