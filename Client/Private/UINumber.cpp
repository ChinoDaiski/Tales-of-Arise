#include "stdafx.h"
#include"UINumber.h"
#include"UIGlow.h"
#include "Player_Manager.h"
#include "Player.h"
#include"Mantis.h"
#include <string>
CUINumber::CUINumber(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUI(pDevice, pDeviceContext)
{
}

CUINumber::CUINumber(const CUINumber& rhs)
	: CUI(rhs)
{
}

HRESULT CUINumber::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUINumber::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		return E_FAIL;
	}

	numKind = ((UINUMDESC*)pArg)->kind;
	m_iSprite = 0;
	if (FAILED(SetUp_Texture()))
	{
		return E_FAIL;
	}

	m_iShaderPass = 3;
	m_fDistanceToCamera = 89.3f;
	return S_OK;
}

void CUINumber::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();
	CBattle_Manager* pBattleManager = CBattle_Manager::GetInstance();

	Safe_AddRef(pUIManager);
	Safe_AddRef(pPlayerManager);
	Safe_AddRef(pBattleManager);

	CPlayer* player = pPlayerManager->Get_MainPlayer();

	if (numKind == 0) {
		int combohit = player->Get_PlayerInfo().m_iComboHits;
		m_str = to_string(player->Get_PlayerInfo().m_iComboHits);

		if (m_tUIInfo.fCX > 50) {


			//Hit
			//m_iSprite++;
			m_tUIInfo.fCX = 40;
			m_tUIInfo.fCY = 40;
		}
		m_tUIInfo.fCX += 1;
		m_tUIInfo.fCY += 1;

	}
	else if (numKind == 1)
	{
		//damage
		damage += player->Get_PlayerInfo().m_iComboHits;
		m_str = to_string(damage);

		if (m_tUIInfo.fCX > 20) {
			m_iSprite++;
			m_tUIInfo.fCX = 10;
			m_tUIInfo.fCY = 10;
		}
		m_tUIInfo.fCX += 1;
		m_tUIInfo.fCY += 1;
	}
	else if (numKind == 2)
	{
		int maxcp = pBattleManager->GetMAXCP();
		m_str = to_string(maxcp);
	}
	else if (numKind == 3)
	{
		int cp = pBattleManager->GetCP();

		if (cp < 0) cp = 0;
		m_str = to_string(cp);

	}
	else if (numKind == 4)
	{
		m_iSprite = 10;

	}
	else if (numKind == 5)
	{




		int hp = player->Get_TargetEnemyHP();
		m_str = to_string(player->Get_TargetEnemyHP());
		int size = m_str.size();


	}

	Safe_Release(pUIManager);
	Safe_Release(pPlayerManager);
}

void CUINumber::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

}

HRESULT CUINumber::Render()
{

	if (numKind == 5)
	{
		for (int i = 0; i < m_str.size(); ++i)
		{

			//종류 별로 뺴주삼..

			m_iSprite = m_str.at(i) - '0';
			_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Pos = XMVectorSetX(Pos, XMVectorGetX(Pos) + 10.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

			if (FAILED(__super::Render()))
			{
				return E_FAIL;
			}
		}
	}
	else if (numKind == 2)
	{
		for (int i = 0; i < m_str.size(); ++i)
		{

			//종류 별로 뺴주삼..

			m_iSprite = m_str.at(i) - '0';
			_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Pos = XMVectorSetX(Pos, XMVectorGetX(Pos) + 19.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

			if (FAILED(__super::Render()))
			{
				return E_FAIL;
			}
		}

	}
	else if (numKind == 3)
	{
		for (int i = 0; i < m_str.size(); ++i)
		{

			//종류 별로 뺴주삼..

			m_iSprite = m_str.at(i) - '0';
			_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Pos = XMVectorSetX(Pos, XMVectorGetX(Pos) + 19.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

			if (FAILED(__super::Render()))
			{
				return E_FAIL;
			}
		}

	}
	else if (numKind == 4)
	{
		if (FAILED(__super::Render()))
		{
			return E_FAIL;
		}
	}
	else if (numKind == 1)
	{
		for (int i = 0; i < m_str.size(); ++i)
		{

			//종류 별로 뺴주삼..

			m_iSprite = m_str.at(i) - '0';
			_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Pos = XMVectorSetX(Pos, XMVectorGetX(Pos) + 15.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

			if (FAILED(__super::Render()))
			{
				return E_FAIL;
			}
		}
	}
	else if (numKind == 0)
	{
		for (int i = 0; i < m_str.size(); ++i)
		{

			//종류 별로 뺴주삼..

			m_iSprite = m_str.at(i) - '0';
			_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Pos = XMVectorSetX(Pos, XMVectorGetX(Pos) + 40.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

			if (FAILED(__super::Render()))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CUINumber::SetUp_Texture()
{
	if (FAILED(__super::SetUp_Components(L"Com_Texture", LEVEL_STATIC, L"Prototype_Component_Texture_Font", (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX(L"Failed To UINumber : SetUp_Texture : SetUp_Component(Com_Texture)");
		return E_FAIL;
	}
	return S_OK;
}

CUINumber * CUINumber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUINumber* pInstance = new CUINumber(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To UINumber : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUINumber::Clone(void * pArg)
{
	CUINumber* pInstance = new CUINumber(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To UINumber : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUINumber::Free()
{
	__super::Free();

}
