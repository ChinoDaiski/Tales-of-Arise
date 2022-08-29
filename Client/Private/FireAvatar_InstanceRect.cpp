#include "stdafx.h"
#include "..\Public\FireAvatar_InstanceRect.h"
#include "GameInstance.h"

CFireAvatar_InstanceRect::CFireAvatar_InstanceRect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CBlendObject(pDevice, pDeviceContext)
{
}

CFireAvatar_InstanceRect::CFireAvatar_InstanceRect(const CFireAvatar_InstanceRect& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CFireAvatar_InstanceRect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : NativeConstruct_Prototype : NativeConstruct_Prototype", E_FAIL);

	return S_OK;
}

HRESULT CFireAvatar_InstanceRect::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : NativeConstruct : NativeConstruct", E_FAIL);

	memcpy(&m_tFireInstanceDesc, pArg, sizeof(FIREINSTANCEDESC));

	if (FAILED(SetUp_Components()))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : NativeConstruct : SetUp_Components", E_FAIL);

	if (FAILED(BufferSet()))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : NativeConstruct : SetUp_Components", E_FAIL);

	m_PassTime = m_EffectDesc->fPassTime;

	m_pSocketMatrix = m_tFireInstanceDesc.pSocketModel->Get_CombinedTransformationMatrix(m_tFireInstanceDesc.tagBone);
	return S_OK;
}

void CFireAvatar_InstanceRect::Tick(_double TimeDelta)
{
	m_bDead = m_bFinish;

	__super::Tick(TimeDelta);
	m_PassTime -= TimeDelta * m_fmultipleTime;

	_matrix PivotMatrix = m_tFireInstanceDesc.pSocketModel->Get_PivotMatrix();
	_matrix WorldMatrix = m_tFireInstanceDesc.pTransformCom->Get_WorldMatrix();

	_vector vRevisePos = (XMLoadFloat4x4(m_pSocketMatrix) * PivotMatrix * WorldMatrix).r[3] + XMLoadFloat3(&m_tFireInstanceDesc.vRevisePos);
	_float4 vPos;
	XMStoreFloat4(&vPos, vRevisePos);
	Set_Pos(vPos);
}

void CFireAvatar_InstanceRect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom&& m_PassTime <= 0)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
		if (m_pVIBufferCom->Get_Billboard())
		{
			Compute_CamDistance();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_bFinish = m_pVIBufferCom->Update(TimeDelta * m_fmultipleTime, XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			Compute_CamDistance();
			m_bFinish = m_pVIBufferCom->Update(TimeDelta * m_fmultipleTime);
		}
	}
}

HRESULT CFireAvatar_InstanceRect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : Render : nullptr", E_FAIL);

	if (FAILED(__super::Render()))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : Render : Render", E_FAIL);

	if (FAILED(SetUp_ConstantTable()))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : Render : SetUp_ConstantTable", E_FAIL);

	m_pShaderCom->Begin(m_EffectDesc->iShader);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFireAvatar_InstanceRect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_Components : SetUp_Components(Com_Renderer)", E_FAIL);

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"), (CComponent**)&m_pShaderCom)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_Components : SetUp_Components(Com_Shader)", E_FAIL);

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), (CComponent**)&m_pTextureCom)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_Components : SetUp_Components(Com_Texture)", E_FAIL);

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance"), (CComponent**)&m_pVIBufferCom, m_tFireInstanceDesc.pEffectDesc)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_Components : SetUp_Components(Com_VIBuffer)", E_FAIL);

	m_EffectDesc = m_pVIBufferCom->Get_EffectDesc();

	return S_OK;
}

HRESULT CFireAvatar_InstanceRect::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Bind_WorldMatrixOnShader(g_WorldMatrix)", E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_ProjMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color1", &m_EffectDesc->vColor1, sizeof(_float4))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_Color1)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color2", &m_EffectDesc->vColor2, sizeof(_float4))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_Color2)", E_FAIL);

	_float2 vBlur = _float2(1.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_Blur", &vBlur, sizeof(_float2))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_Blur)", E_FAIL);

	_float m_f = m_pVIBufferCom->Get_Time() / m_pVIBufferCom->Get_MaxTime();
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_f, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : Set_RawValue(g_Alpha)", E_FAIL);

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", m_EffectDesc->iTexture)))
		MSG_CHECK_RETURN(L"Failed To CFireAvatar_InstanceRect : SetUp_ConstantTable : SetUp_ShaderResourceView(g_DiffuseTexture)", E_FAIL);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CFireAvatar_InstanceRect::BufferSet()
{
	m_EffectDesc = m_pVIBufferCom->Get_EffectDesc();
	m_pVIBufferCom->Restart();
	return S_OK;
}

CFireAvatar_InstanceRect* CFireAvatar_InstanceRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFireAvatar_InstanceRect* pInstance = new CFireAvatar_InstanceRect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To CFireAvatar_InstanceRect : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFireAvatar_InstanceRect::Clone(void* pArg)
{
	CFireAvatar_InstanceRect* pInstance = new CFireAvatar_InstanceRect(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To CFireAvatar_InstanceRect : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireAvatar_InstanceRect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}