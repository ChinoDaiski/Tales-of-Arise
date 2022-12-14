#include "stdafx.h"
#include "..\Public\Rect_Effect.h"
#include "GameInstance.h"
#include "Enemy.h"
#include "Player.h"

CRect_Effect::CRect_Effect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBlendObject(pDeviceOut, pDeviceContextOut)
{

}

CRect_Effect::CRect_Effect(const CRect_Effect & rhs)
	: CBlendObject(rhs)
{
}

HRESULT CRect_Effect::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRect_Effect::NativeConstruct(void * pArg)
{


	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(BufferSet()))
		return E_FAIL;
	m_PassTime = m_EffectDesc->fPassTime;
	return S_OK;
}

void CRect_Effect::Tick(_double TimeDelta)
{


	if (m_Parents_P)
	{
		if (m_Unit_Type == UNIT_MONSTER)
		{
			if (((CEnemy*)m_Parents_P)->Get_Hp() <= 0)
				m_bDead = true;
		}
		else if (m_Unit_Type == UNIT_PLAYER)
		{
			if (((CPlayer*)m_Parents_P)->Get_Hp() <= 0)
				m_bDead = true;
		}
		else if (m_Unit_Type == UNIT_BOSS)
		{
			if (((CEnemy*)m_Parents_P)->Get_Hp() <= 0)
				m_bDead = true;
		}
	}



	__super::Tick(TimeDelta);
	m_PassTime -= TimeDelta * m_fmultipleTime*m_dTimeSpeed;
}

void CRect_Effect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	if (!m_bDead)
		m_bDead = m_bFinish;



	if (nullptr != m_pRendererCom&& m_PassTime <= 0)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
		if (m_pVIBufferCom->Get_Billboard())
		{
			Compute_CamDistance();
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			m_bFinish = m_pVIBufferCom->Update(TimeDelta * m_fmultipleTime*m_dTimeSpeed, XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			Compute_CamDistance();
			m_bFinish = m_pVIBufferCom->Update(TimeDelta * m_fmultipleTime*m_dTimeSpeed);
		}

	}




}

HRESULT CRect_Effect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(m_EffectDesc->iShader);

	m_pVIBufferCom->Render();


	return S_OK;
}


HRESULT CRect_Effect::BufferSet()
{
	m_EffectDesc = m_pVIBufferCom->Get_EffectDesc();
	m_pVIBufferCom->Restart();
	return S_OK;
}

void CRect_Effect::Set_TimeEffect(_float Time)
{
	m_pVIBufferCom->Set_Time(Time);
}

HRESULT CRect_Effect::SetUp_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance"), (CComponent**)&m_pVIBufferCom, pArg)))
		return E_FAIL;
	m_EffectDesc = m_pVIBufferCom->Get_EffectDesc();


	return S_OK;
}

HRESULT CRect_Effect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_Parents_P != nullptr)
	{
		_matrix	MX = m_Parents_TF->Get_WorldMatrix();
		MX.r[3] += (MX.r[0] * m_Parents_RUL.x) + (MX.r[1] * m_Parents_RUL.y) + (MX.r[2] * m_Parents_RUL.z);
		//XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_LocalMatrix) * MX));
		m_pTransformCom->Set_WorldMatrix(MX);
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color1", &m_EffectDesc->vColor1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color2", &m_EffectDesc->vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Blur", &m_Blur, sizeof(_float2))))
		return E_FAIL;


	_float m_f = m_pVIBufferCom->Get_Time() / m_pVIBufferCom->Get_MaxTime();
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_f, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", m_EffectDesc->iTexture)))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CRect_Effect::Set_Transform(UNIT_TYPE Type, CGameObject* OBJ, CTransform * TF, _float3 RUL)
{
	m_Unit_Type = Type;
	m_Parents_P = OBJ;
	m_Parents_TF = TF;
	m_Parents_RUL = RUL;
}

CRect_Effect * CRect_Effect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CRect_Effect*	pInstance = new CRect_Effect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRect_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBlendObject * CRect_Effect::Clone(void * pArg)
{
	CRect_Effect*	pInstance = new CRect_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRect_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRect_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
