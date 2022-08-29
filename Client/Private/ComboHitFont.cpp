#include "stdafx.h"
#include "..\Public\ComboHitFont.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include"Player.h"
#include"Player_Manager.h"
#include"Enemy.h"
#include<string>
ComboHitFont::ComboHitFont(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

ComboHitFont::ComboHitFont(const ComboHitFont & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT ComboHitFont::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	m_fWinCX = g_iWinCX;
	m_fWinCY = g_iWinCY;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fWinCX, m_fWinCY, 0.f, 1.f));

	return S_OK;
}

HRESULT ComboHitFont::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC      TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	TargetTransform = ((MHDESC*)pArg)->targetTransform;
	m_pivotx = ((MHDESC*)pArg)->pivotx;
	m_pivoty = ((MHDESC*)pArg)->pivoty;
	m_ScaleX = ((MHDESC*)pArg)->scaleX;
	m_ScaleY = ((MHDESC*)pArg)->scaleY;
	m_kind = ((MHDESC*)pArg)->kind;
	m_UIKind = ((MHDESC*)pArg)->UIKind;
	m_HitDamage = ((MHDESC*)pArg)->HitDamage;
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	m_DamageStr = to_string(m_HitDamage); //걍 일단은 랜덤...


	return S_OK;
}

void ComboHitFont::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();
	Safe_AddRef(pPlayerManager);
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

	m_pTransformCom->Scaled(_float3(m_ScaleX, m_ScaleY, 1.f));


	CUI_Manager* pUiManager = CUI_Manager::GetInstance();
	

	Safe_AddRef(pUiManager);
	
	////아몰랑 공격력여기다가 띄워줄거임 돼지시키 내지는 멘티스 녀석 공격력을 string으로 바꿔준다.
	//여기가 문제같아 


	//삭제가자
	m_ftime += TimeDelta;
	if (m_ftime > 0.5f)
		Set_Dead(1);
	
	Safe_Release(pPlayerManager);
	Safe_Release(pUiManager);

}

void ComboHitFont::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
	
}

HRESULT ComboHitFont::Render()
{
	
		for (int i = 0; i < m_DamageStr.size(); ++i)
		{

			//종류 별로 뺴주삼..
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f+(i*30), -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
			m_kind = m_DamageStr.at(i) - '0';

		if (nullptr == m_pShaderCom ||
			nullptr == m_pVIBufferCom)
			return E_FAIL;

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
		if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", m_kind)))
			return E_FAIL;


		//윈도우 -> 다음에 직교투영
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));




		//apply 하기전에 값을 던저야하므로 begin전에 숫자를 던지자.
		if (FAILED(m_pShaderCom->Begin(26)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		}
		return S_OK;
}

HRESULT ComboHitFont::SetUp_Components()
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


	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Font"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

ComboHitFont * ComboHitFont::Create(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
{
	ComboHitFont*	pInstance = new ComboHitFont(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created PlayerMovingHP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * ComboHitFont::Clone(void * pArg)
{
	ComboHitFont*	pInstance = new ComboHitFont(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created ComboHitFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ComboHitFont::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
