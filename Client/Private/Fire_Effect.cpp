#include "stdafx.h"
#include "..\Public\Fire_Effect.h"
#include "GameInstance.h"

CFire_Effect::CFire_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CBlendObject(pDevice, pDeviceContext)
{
}

CFire_Effect::CFire_Effect(const CFire_Effect& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CFire_Effect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CFire_Effect::NativeConstruct(void* pArg)
{
	if (nullptr == pArg)
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : NativeConstruct : nullptr == pArg", E_FAIL);

	if (FAILED(__super::NativeConstruct(pArg)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : NativeConstruct : NativeConstruct", E_FAIL);

	memcpy(&m_tFireEffectDesc, pArg, sizeof(FIREEFFECTDESC));

	if (FAILED(SetUp_Components(m_tFireEffectDesc.tagTextureCom)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : NativeConstruct : SetUp_Components", E_FAIL);

	m_iMaxFrame = m_pTextureCom->Get_TextureCnt();

	_matrix SocketMatrix = XMMatrixIdentity();
	SocketMatrix.r[3] = XMLoadFloat4x4(m_tFireEffectDesc.pSocketModel->Get_CombinedTransformationMatrix(m_tFireEffectDesc.tagBone)).r[3];
	_matrix PivotMatrix = m_tFireEffectDesc.pSocketModel->Get_PivotMatrix();
	_matrix ParentMatrix = m_tFireEffectDesc.pTransformCom->Get_WorldMatrix();

	_matrix WorldMatrix = SocketMatrix * PivotMatrix * ParentMatrix;
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	WorldMatrix.r[2] = XMVector3Normalize(XMVectorSetY(WorldMatrix.r[2], 0.f)) * m_tFireEffectDesc.vScale.z;
	WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_tFireEffectDesc.vScale.y;
	WorldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2])) * m_tFireEffectDesc.vScale.x;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (true == m_tFireEffectDesc.bRandom)
	{
		m_fReviceX = m_pTransformCom->Get_Scale().x * Get_RandomFloat(-0.3f, 0.3f);
	}
	else
	{
		m_fReviceX = 0.f;
	}

	if (2 == m_tFireEffectDesc.iShaderPass)
	{
		m_fReviceY = m_pTransformCom->Get_Scale().y * 0.8f;
	}
	else if (7 == m_tFireEffectDesc.iShaderPass)
	{
		m_fReviceY = m_pTransformCom->Get_Scale().y * 0.4f;
	}

	WorldMatrix.r[3] += XMVectorSet(m_fReviceX, m_fReviceY, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);

	FrameSpeed = m_tFireEffectDesc.FrameSpeed;

	return S_OK;
}

void CFire_Effect::Tick(_double TimeDelta)
{
	m_Frame += FrameSpeed * TimeDelta;
	if (m_iMaxFrame <= m_Frame)
	{
		Restart_FireEffect();
	}

	if (true == m_tFireEffectDesc.bChase)
	{
		_matrix SocketMatrix = XMLoadFloat4x4(m_tFireEffectDesc.pSocketModel->Get_CombinedTransformationMatrix(m_tFireEffectDesc.tagBone));
		_matrix PivotMatrix = m_tFireEffectDesc.pSocketModel->Get_PivotMatrix();
		_matrix ParentMatrix = m_tFireEffectDesc.pTransformCom->Get_WorldMatrix();

		_vector vPos = (SocketMatrix * PivotMatrix * ParentMatrix).r[3];
		vPos += XMVectorSet(0.f, m_fReviceY, 0.f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CFire_Effect::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
	{
		return;
	}
	Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CFire_Effect::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : Render : SetUp_ConstantTable", E_FAIL);

	m_pShaderCom->Begin(m_tFireEffectDesc.iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFire_Effect::SetUp_Components(_tchar* tagTextureCom)
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_Components : SetUp_Components(Com_Renderer)", E_FAIL);

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Fire"), (CComponent**)&m_pShaderCom)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_Components : SetUp_Components(Com_Shader)", E_FAIL);

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_Components : SetUp_Components(Com_VIBuffer)", E_FAIL);

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, tagTextureCom, (CComponent**)&m_pTextureCom)))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_Components : SetUp_Components(Com_Texture)", E_FAIL);

	return S_OK;
}

HRESULT CFire_Effect::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Bind_WorldMatrixOnShader(g_WorldMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_RawValue(g_ProjMatrix)", E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_OriginalStrength", &m_tFireEffectDesc.fOriginalStrength, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_RawValue(g_OriginalStrength)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_GlowStrength", &m_tFireEffectDesc.fGlowStrength, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_RawValue(g_GlowStrength)", E_FAIL);

	_float fCamFar = pGameInstance->Get_CamFar();
	if (FAILED(m_pShaderCom->Set_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_RawValue(g_fCamFar)", E_FAIL);

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_RenderTargetSRV(TEXT("Target_Depth")))))
		MSG_CHECK_RETURN(L"Failed To CFire_Effect : SetUp_ConstantTable : Set_ShaderResourceView(g_DepthTexture)", E_FAIL);
	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_ImageTexture", (_uint)m_Frame)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFire_Effect::Restart_FireEffect()
{
	m_Frame = 0.0;

	_float3 vScale = m_tFireEffectDesc.vScale;
	vScale.x = vScale.x * Get_RandomFloat(0.7f, 1.3f);
	vScale.y = vScale.y * Get_RandomFloat(0.7f, 1.3f);

	_matrix SocketMatrix = XMMatrixIdentity();
	SocketMatrix.r[3] = XMLoadFloat4x4(m_tFireEffectDesc.pSocketModel->Get_CombinedTransformationMatrix(m_tFireEffectDesc.tagBone)).r[3];
	_matrix PivotMatrix = m_tFireEffectDesc.pSocketModel->Get_PivotMatrix();
	_matrix ParentMatrix = m_tFireEffectDesc.pTransformCom->Get_WorldMatrix();

	_matrix WorldMatrix = SocketMatrix * PivotMatrix * ParentMatrix;
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	WorldMatrix.r[2] = XMVector3Normalize(XMVectorSetY(WorldMatrix.r[2], 0.f)) * vScale.z;
	WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	WorldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2])) * vScale.x;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (true == m_tFireEffectDesc.bRandom)
	{
		m_fReviceX = m_pTransformCom->Get_Scale().x * Get_RandomFloat(-0.3f, 0.3f);
	}
	else
	{
		m_fReviceX = 0.f;
	}

	if (2 == m_tFireEffectDesc.iShaderPass)
	{
		m_fReviceY = m_pTransformCom->Get_Scale().y * 0.8f;
	}
	else if (7 == m_tFireEffectDesc.iShaderPass)
	{
		m_fReviceY = m_pTransformCom->Get_Scale().y * 0.4f;
	}

	WorldMatrix.r[3] += XMVectorSet(m_fReviceX, m_fReviceY, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);

	FrameSpeed = m_tFireEffectDesc.FrameSpeed + (rand() % 5 - 2);

	return S_OK;
}

CFire_Effect* CFire_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFire_Effect* pInstance = new CFire_Effect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To CFire_Effect : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFire_Effect::Clone(void* pArg)
{
	CFire_Effect* pInstance = new CFire_Effect(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To CFire_Effect : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFire_Effect::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}