#include "stdafx.h"
#include "..\Public\Picking_Rect.h"

CPicking_Rect::CPicking_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPicking_Rect::CPicking_Rect(const CPicking_Rect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPicking_Rect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CPicking_Rect -> NativeConstruct_Prototype -> NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPicking_Rect::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	Picking_RectDESC* Desc = (Picking_RectDESC*)pArg;

	_float3 fPos = Desc->vPos; 
	_float2	fSize = Desc->fSize;

	m_RectPos[0] = _float3(
		fPos.x - fSize.x * 0.5f,
		fPos.y,
		fPos.z + fSize.y * 0.5f
	);
	m_RectPos[1] = _float3(
		fPos.x + fSize.x * 0.5f,
		fPos.y,
		fPos.z + fSize.y * 0.5f
	);
	m_RectPos[2] = _float3(
		fPos.x + fSize.x * 0.5f,
		fPos.y,
		fPos.z - fSize.y * 0.5f
	);
	m_RectPos[3] = _float3(
		fPos.x - fSize.x * 0.5f,
		fPos.y,
		fPos.z - fSize.y * 0.5f
	);

	m_vCenterPos = fPos;
	m_iSizeX = (_uint)fSize.x;
	m_iSizeY = (_uint)fSize.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vCenterPos), 1.f));
	m_pTransformCom->Scaled(_float3((_float)m_iSizeX, (_float)m_iSizeY, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(90.f));

	return S_OK;
}

void CPicking_Rect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CPicking_Rect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPicking_Rect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(20))) //¼º¿í¾Æ ³»°¡ ¹Ù²å¾î ³ÊÀÇ pass´Â 20ÀÓ - ¹ÎÈñ
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPicking_Rect::SetUp_Components(void * pArg)
{
	if (FAILED(__super::SetUp_Components(L"Com_Renderer", LEVEL_STATIC, L"Prototype_Component_Renderer", (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX(L"Failed To CUI : SetUp_Components_Basic : SetUp_Component(Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(L"Com_VIBuffer", LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", (CComponent**)&m_pVIBufferCom)))
	{
		MSG_BOX(L"Failed To CUI : SetUp_Components_Basic : SetUp_Component(Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_MapTool, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPicking_Rect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPicking_Rect::Change_Height(_float fHeight)
{
	m_RectPos[0].y = fHeight;
	m_RectPos[1].y = fHeight;
	m_RectPos[2].y = fHeight;
	m_RectPos[3].y = fHeight;

	m_vCenterPos.y = fHeight;

	_vector vPos = XMVectorSet(m_vCenterPos.x, fHeight, m_vCenterPos.z, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

CPicking_Rect* CPicking_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPicking_Rect* pInstance = new CPicking_Rect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CPicking_Rect -> Create -> pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPicking_Rect::Clone(void* pArg)
{
	CPicking_Rect* pInstance = new CPicking_Rect(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"CPicking_Rect -> CMinion_Warrior -> pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPicking_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}