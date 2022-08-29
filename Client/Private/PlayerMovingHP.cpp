#include "stdafx.h"
#include "PlayerMovingHP.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "Player.h"
#include "Alphen.h"
#include "Shionne.h"

//#include "DiamondHp.h"
//#include"ComboHitFont.h"
//#include "NotMovingFont.h"

PlayerMovingHP::PlayerMovingHP(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

PlayerMovingHP::PlayerMovingHP(const PlayerMovingHP & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)

{
}

HRESULT PlayerMovingHP::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	m_fWinCX = g_iWinCX;
	m_fWinCY = g_iWinCY;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fWinCX, m_fWinCY, 0.f, 1.f));

	return S_OK;
}

HRESULT PlayerMovingHP::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC      TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	TargetTransform = ((MHDESC*)pArg)->targetTransform;
	m_pivotx = ((MHDESC*)pArg)->pivotx;
	m_pivoty = ((MHDESC*)pArg)->pivoty;
	m_UIKind = ((MHDESC*)pArg)->UIKind;

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;



	return S_OK;
}

void PlayerMovingHP::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_matrix ViewMatrix, ProjMatrix;
	_float4x4 WindowMatrix, WorldMatrix;

	ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	//이곳을 주목하라.

	XMStoreFloat4x4(&WorldMatrix, TargetTransform->Get_WorldMatrix());

	ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4x4(&WindowMatrix, XMLoadFloat4x4(&WorldMatrix)*ViewMatrix*ProjMatrix);

	_float3 vWindowPos = *(_float3*)&WindowMatrix.m[3][0];

	vWindowPos.x /= vWindowPos.z;
	vWindowPos.y /= vWindowPos.z;
	m_fX = vWindowPos.x*g_iWinCX / 2 + g_iWinCX / 2 + m_pivotx;
	m_fY = vWindowPos.y*-1 * g_iWinCY / 2 + g_iWinCY / 2 + m_pivoty;

	m_pTransformCom->Scaled(_float3(200, 12, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	CUI_Manager* pUiManager = CUI_Manager::GetInstance();
	Safe_AddRef(pUiManager);

	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();

	Safe_AddRef(pPlayerManager);


	CBattle_Manager* pBattleManager = CBattle_Manager::GetInstance();
	Safe_AddRef(pBattleManager);
	CPlayer* player = pUiManager->GetPlayer(pPlayerManager->Get_MainPlayerIndex());


	//if (m_UIKind == 0) // Player일떄, 
	//{
	//



	//	
	//}
	//else if (m_UIKind == 1) //몬스터야
	//{

	//	m_isMonster = true;
	//

	//	//_float fHP = (_float)player->Get_TargetEnemyHP() / player->Get_TargetEnemyMaxHP();
	//	int hp = player->Get_TargetEnemyHP();
	//	int hpmax = player->Get_TargetEnemyMaxHP();
	//	percent =hp / hpmax;
	//	if (m_Redbar > percent)
	//	{
	//		m_Redbar -= TimeDelta / 5.f;
	//	}
	//	if (m_Redbar < percent)
	//		m_Redbar = percent;
	//}




	m_isMonster = false;
	CPlayer* pPlayer = pPlayerManager->Get_MainPlayer();
	pPlayer->Get_PlayerInfo().m_iMaxHp;
	CPlayer::TagPlayerInfo PlayerInfo = player->Get_PlayerInfo();
	_float fHP = (_float)PlayerInfo.m_iCurrentHp / PlayerInfo.m_iMaxHp;

	percent = fHP;
	if (m_Redbar > percent)
	{
		m_Redbar -= TimeDelta / 5.f;
	}
	if (m_Redbar < percent)
		m_Redbar = percent;


	Safe_Release(pUiManager);

	Safe_Release(pPlayerManager);

}

void PlayerMovingHP::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT PlayerMovingHP::Render()
{

	//if (m_UIKind == 0)
	//{

	//}
	//else if (m_UIKind == 1)
	//{
	//	if (nullptr == m_pShaderCom ||
	//		nullptr == m_pVIBufferCom)
	//		return E_FAIL;

	//	m_pShaderCom->Set_RawValue("playermovingpercent", &percent, sizeof(_float));
	//	m_pShaderCom->Set_RawValue("playerrendbar", &m_Redbar, sizeof(_float));
	//	m_pShaderCom->Set_RawValue("g_isMonster", &m_isMonster, sizeof(bool));


	//	if (FAILED(__super::Render()))
	//		return E_FAIL;

	//	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
	//		return E_FAIL;


	//	_float4x4   ViewMatrix;
	//	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//		return E_FAIL;
	//	_float4x4   ProjMatrixTP;
	//	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
	//		return E_FAIL;
	//	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
	//		return E_FAIL;


	//	//윈도우 -> 다음에 직교투영
	//	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));


	//	//apply 하기전에 값을 던저야하므로 begin전에 숫자를 던지자.
	//	if (FAILED(m_pShaderCom->Begin(26)))
	//		return E_FAIL;

	//	if (FAILED(m_pVIBufferCom->Render()))
	//		return E_FAIL;


	//}


	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pShaderCom->Set_RawValue("playermovingpercent", &percent, sizeof(_float));
	m_pShaderCom->Set_RawValue("playerrendbar", &m_Redbar, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_isMonster", &m_isMonster, sizeof(bool));


	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4   ProjMatrixTP;
	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	//윈도우 -> 다음에 직교투영
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));


	//apply 하기전에 값을 던저야하므로 begin전에 숫자를 던지자.
	if (FAILED(m_pShaderCom->Begin(28)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT PlayerMovingHP::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	//Prototype_Component_Texture_HPBar
	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_HPBar"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

PlayerMovingHP * PlayerMovingHP::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	PlayerMovingHP*	pInstance = new PlayerMovingHP(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created PlayerMovingHP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PlayerMovingHP::Clone(void * pArg)
{
	PlayerMovingHP*	pInstance = new PlayerMovingHP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created PlayerMovingHP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PlayerMovingHP::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
