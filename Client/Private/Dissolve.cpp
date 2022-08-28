#include "stdafx.h"
#include "..\Public\Dissolve.h"
#include "GameInstance.h"

CDissolve::CDissolve(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDissolve::CDissolve(const CDissolve & rhs)
	:CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
	, m_fWinCX(rhs.m_fWinCX)
	, m_fWinCY(rhs.m_fWinCY)
{
}

HRESULT CDissolve::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	m_fWinCX = g_iWinCX;
	m_fWinCY = g_iWinCY;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fWinCX, m_fWinCY, 0.f, 1.f));
	return S_OK;
}

HRESULT CDissolve::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
	{
		MSG_BOX(L"Failed To CDissolve : NativeConstruct : NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX(L"Failed To CDissolve : NativeConstruct : SetUp_Components");
		return E_FAIL;
	}	

	return S_OK;
}

void CDissolve::Tick(_double TimeDelta)
{
	if (true == m_bStart)
	{
		m_DissolveTimeAcc += TimeDelta;
	}
	if (m_DissolveTime <= m_DissolveTimeAcc)
	{
		m_bStart = false;
		m_DissolveTimeAcc = 0.0;
	}

	m_pTransformCom->Scaled(_float3(m_fWinCX, m_fWinCY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((m_fWinCX * 0.5f) - g_iWinCX * 0.5f, -(m_fWinCY * 0.5f) + g_iWinCY * 0.5f, 0.f, 1.f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (true == m_pRendererCom->Get_DissolveStart())
	{
		m_pRendererCom->Set_DissolveStart(false);
	}
	if (false == m_pRendererCom->Get_DissolveStart() && pGameInstance->Get_DIKeyState(DIK_RETURN) & 0x80)
	{
		m_pRendererCom->Set_DissolveStart(true);
		m_bStart = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDissolve::LateTick(_double TimeDelta)
{
	if (true == m_bStart)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
	}
}

HRESULT CDissolve::Render()
{
	if (FAILED(SetUp_ConstantTable()))
	{
		MSG_BOX(L"Failed To CDissolve : Render : SetUp_ConstantTable");
		return E_FAIL;
	}

	m_pShaderCom->Begin(24);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDissolve::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(L"Com_Renderer", LEVEL_STATIC, L"Prototype_Component_Renderer", (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX(L"Failed To CDissolve : SetUp_Components : SetUp_Component(Com_Renderer)");
		return E_FAIL;
	}
	if (FAILED(__super::SetUp_Components(L"Com_Shader", LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex", (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX(L"Failed To CDissolve : SetUp_Components : SetUp_Component(Com_Shader)");
		return E_FAIL;
	}
	if (FAILED(__super::SetUp_Components(L"Com_VIBuffer", LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", (CComponent**)&m_pVIBufferCom)))
	{
		MSG_BOX(L"Failed To CDissolve : SetUp_Components : SetUp_Component(Com_VIBuffer)");
		return E_FAIL;
	}
	if (FAILED(__super::SetUp_Components(L"Com_SourDissolveTexture", LEVEL_STATIC, L"Prototype_Component_Texture_Dissolve_Sour", (CComponent**)&m_pSourDissolveTextureCom)))
	{
		MSG_BOX(L"Failed To CDissolve : SetUp_Components : SetUp_Component(Com_SourDissolveTexture)");
		return E_FAIL;
	}
	if (FAILED(__super::SetUp_Components(L"Com_DestDissolveTexture", LEVEL_STATIC, L"Prototype_Component_Texture_Dissolve_Dest", (CComponent**)&m_pDestDissolveTextureCom)))
	{
		MSG_BOX(L"Failed To CDissolve : SetUp_Components : SetUp_Component(Com_DestDissolveTexture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDissolve::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pShader->Set_RawValue(g_ViewMatrix)");
		return E_FAIL;
	}
	_float4x4 ProjMatrixTP;
	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pShader->Set_RawValue(g_ProjMatrix)");
		return E_FAIL;
	}

	_float fDissolveTime = _float(m_DissolveTimeAcc / m_DissolveTime);
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pShader->Set_RawValue(g_fDissolveTime)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor", &m_vDissolveColor, sizeof(_float4))))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pShader->Set_RawValue(g_vDissolveColor)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_Texture", pGameInstance->Get_RenderTargetSRV(L"Target_Dissolve"))))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pShader->m_pShaderCom->Set_ShaderResourceView(g_Texture)");
		return E_FAIL;
	}

	if (FAILED(m_pSourDissolveTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_SourDissolveTexture", 4)))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pDissolveTextureCom->SetUp_ShaderResourceView(g_SourDissolveTexture)");
		return E_FAIL;
	}
	if (FAILED(m_pDestDissolveTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DestDissolveTexture", 0)))
	{
		MSG_BOX(L"Failed To CDissolve : Render : m_pDissolveTextureCom->SetUp_ShaderResourceView(g_DestDissolveTexture)");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDissolve * CDissolve::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDissolve* pInstance = new CDissolve(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To CDissolve : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDissolve::Clone(void * pArg)
{
	CDissolve* pInstance = new CDissolve(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To CDissolve : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDissolve::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSourDissolveTextureCom);
	Safe_Release(m_pDestDissolveTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}