#include "stdafx.h"
#include "..\Public\Distortion.h"
#include "GameInstance.h"

CDistortion::CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDistortion::CDistortion(const CDistortion& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDistortion::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CDistortion::NativeConstruct(void* pArg)
{
	if (nullptr == pArg)
		MSG_CHECK_RETURN(L"Failed To CDistortion : NativeConstruct : nullptr == pArg", E_FAIL);

	CTransform::TRANSFORMDESC tTransformDesc;
	ZeroMemory(&tTransformDesc, sizeof(CTransform::TRANSFORMDESC));
	tTransformDesc.vScalePerSec = _float3(5.f, 5.f, 5.f);

	if(FAILED(__super::NativeConstruct(pArg, &tTransformDesc)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : NativeConstruct : NativeConstruct", E_FAIL);

	memcpy(&m_tDistortionDesc, pArg, sizeof(DISTORTIONDESC));

	if (FAILED(SetUp_Components()))
		MSG_CHECK_RETURN(L"Failed To CDistortion : NativeConstruct : SetUp_Components", E_FAIL);

	CTransform* pTarget_Transform = (CTransform*)m_tDistortionDesc.pTargetObject->Get_Component(L"Com_Transform");
	_matrix WorldMatrix = XMMatrixIdentity();

	WorldMatrix.r[2] = pTarget_Transform->Get_State(CTransform::STATE_LOOK);
	WorldMatrix.r[2] = XMVector3Normalize(XMVectorSetY(WorldMatrix.r[2], 0.f));
	WorldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2]));

	WorldMatrix.r[3] = pTarget_Transform->Get_State(CTransform::STATE_POSITION);
	WorldMatrix.r[3] += WorldMatrix.r[2] * m_tDistortionDesc.vRevicePosition.x + WorldMatrix.r[1] * m_tDistortionDesc.vRevicePosition.y;

	WorldMatrix.r[0] *= m_tDistortionDesc.vScale.x;
	WorldMatrix.r[1] *= m_tDistortionDesc.vScale.y;
	WorldMatrix.r[2] *= m_tDistortionDesc.vScale.z;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

void CDistortion::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return;
	}

	if (true == m_bRender)
	{
		m_pTransformCom->Scaling2(TimeDelta);
	}
	else
	{
		m_pTransformCom->Scaled(m_tDistortionDesc.vScale);
	}

	CTransform* pTarget_Transform = (CTransform*)m_tDistortionDesc.pTargetObject->Get_Component(L"Com_Transform");
	_matrix WorldMatrix = XMMatrixIdentity();

	WorldMatrix.r[2] = pTarget_Transform->Get_State(CTransform::STATE_LOOK);
	WorldMatrix.r[2] = XMVector3Normalize(XMVectorSetY(WorldMatrix.r[2], 0.f));
	WorldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2]));

	WorldMatrix.r[3] = pTarget_Transform->Get_State(CTransform::STATE_POSITION);
	WorldMatrix.r[3] += WorldMatrix.r[2] * m_tDistortionDesc.vRevicePosition.x + WorldMatrix.r[1] * m_tDistortionDesc.vRevicePosition.y;

	_float3 vScale = m_pTransformCom->Get_Scale();
	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;
	WorldMatrix.r[2] *= vScale.z;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CDistortion::LateTick(_double TimeDelta)
{
	if (false == m_bDead && true == m_bRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
	}
}

HRESULT CDistortion::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		MSG_CHECK_RETURN(L"Failed To CDistortion : Render : nullptr", E_FAIL);

	if (FAILED(__super::Render()))
		MSG_CHECK_RETURN(L"Failed To CDistortion : Render : Render", E_FAIL);

	if (FAILED(SetUp_ConstantTable()))
		MSG_CHECK_RETURN(L"Failed To CDistortion : Render : SetUp_ConstantTable", E_FAIL);

	if (FAILED(m_pShaderCom->Begin(25)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : Render : Begin", E_FAIL);

	if (FAILED(m_pVIBufferCom->Render()))
		MSG_CHECK_RETURN(L"Failed To CDistortion : Render : Render", E_FAIL);

	return S_OK;
}

_bool CDistortion::Get_Render()
{
	return m_bRender;
}

void CDistortion::Set_Render(_bool bRender)
{
	m_bRender = bRender;
}

HRESULT CDistortion::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_Components : SetUp_Components(Com_Renderer)", E_FAIL);

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_Components : SetUp_Components(Com_Shader)", E_FAIL);

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_Components : SetUp_Components(Com_VIBuffer)", E_FAIL);

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Distortion"), (CComponent**)&m_pTextureCom)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_Components : SetUp_Components(Com_Texture)", E_FAIL);

	return S_OK;
}

HRESULT CDistortion::SetUp_ConstantTable()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_ConstantTable : Bind_WorldMatrixOnShader(g_WorldMatrix)", E_FAIL);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_ConstantTable : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_ConstantTable : Set_RawValue(g_ProjMatrix)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		MSG_CHECK_RETURN(L"Failed To CDistortion : SetUp_ConstantTable : SetUp_ShaderResourceView(g_Texture)", E_FAIL);

	return S_OK;
}

CDistortion* CDistortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDistortion* pInstance = new CDistortion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed To CDistortion : Create : NativeConstruct_Prototype"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDistortion::Clone(void* pArg)
{
	CDistortion* pInstance = new CDistortion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed To CDistortion : Create : NativeConstruct"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDistortion::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}