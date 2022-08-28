#include "stdafx.h"
#include "..\Public\Owl.h"

#include "GameInstance.h"

#include "Collider.h"
#include "Animation.h"

#include "Player.h"
#include "UI_Owl.h"	

COwl::COwl(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

COwl::COwl(const COwl & rhs)
	: CGameObject(rhs)
{
}

HRESULT COwl::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COwl::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	OwlDesc Desc = *((OwlDesc*)pArg);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&Desc.TransformMatrix));

	m_pPlayer = Desc.pTargetPlayer;

	m_pModelCom->Set_AnimationIndex(m_iCurrentAnimationIndex);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	m_pInteractionUI = dynamic_cast<CUI_Owl*>(pGameInstance->Clone_Prototype(TEXT("Prototype_GameObject_UI_Owl")));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void COwl::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bInRange) {
		m_pModelCom->Update(0, true);
		Check_InRange();
		return;
	}

	if (nullptr != m_pInteractionUI)
		m_pInteractionUI->Tick(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	static bool _bDone = false;

	if (m_iCurrentAnimationIndex == m_iNextAnimationIndex) {
		switch (m_iOwlNumber)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			m_pModelCom->Update(TimeDelta * 2.f, true);
			break;
		case 14:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_AWAY_DIAGONALLY) {
				if (!m_pModelCom->Get_Animation(m_iCurrentAnimationIndex)->Get_Finish()) {
					if (!_bDone) {
						m_pModelCom->Get_Animation(m_iCurrentAnimationIndex)->Reset_Animation();
						_bDone = true;
					}
				}

				m_pModelCom->Update(TimeDelta * 2.f, false);

				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				_vector MoveVector = XMVector3TransformCoord(XMLoadFloat3(&m_pModelCom->Get_CurAnimation()->Get_MoveVector()), m_pModelCom->Get_PivotMatrix());


				//////
				_matrix		BoneMatrix = WorldMatrix;

				BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
				BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
				BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);
				BoneMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

				//_float fLength = XMVectorGetX(XMVector3Length(MoveVector));
				MoveVector = XMVector3TransformCoord(MoveVector, BoneMatrix);// *fLength;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + MoveVector, 1.f));
			}

		default:
			m_pModelCom->Update(TimeDelta * 2.f, false);
			break;
		}
	}
	else {
		m_pModelCom->Update_NextAnim(TimeDelta, 1.0, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
	}


	// 범위내에 플레이어가 있을 때
	if (m_bInRange) {
		// 루프가 true라면
		if (m_bInRoop) {
			// X 키를 눌렀을 경우
			if (pGameInstance->Key_Down(DIK_X)) {
				// 반복되는 애니메이션이 해제되고, 날아가는 애니메이션이 나온다.
				m_bInRoop = false;
			}
		}
	}



	CCollider* pCollider = dynamic_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_SPHERE_Interaction")));

	if (nullptr == pCollider) {
		MSG_BOX(TEXT("COwl : No Interaction Collider in Player"));
		return;
	}

	// 아직 플레이어와 만나지 않은 경우
	if (!m_bEncounter) {
		// 여기서 m_bEncounter변수를 true 로 만든다.
		Check_Collision();

		if (nullptr != m_pInteractionUI)
			m_pInteractionUI->Set_InRange(false);
	}
	// 플레이어와 만난 경우
	else {
		if (nullptr != m_pInteractionUI)
			m_pInteractionUI->Set_InRange(true);


		// 번호에 따라 다음 애니메이션 설정
		switch (m_iOwlNumber)
		{
		case 0:
			m_iNextAnimationIndex = OWL_IN_POSITION_OPEN_WING;
			break;
		case 1:
			m_iNextAnimationIndex = OWL_IN_POSITION_TILT_HEAD;
			break;
		case 2:
			m_iNextAnimationIndex = OWL_IN_POSITION_WINGING_FAST;
			break;
		case 3:
			m_iNextAnimationIndex = OWL_IN_POSITION_WINGING_SLOW;
			break;
		case 4:
			m_iNextAnimationIndex = OWL_IN_POSITION_LOOK_AROUND;
			break;
		case 5:
			m_iNextAnimationIndex = OWL_IN_POSITION_FLAP_WINGS;
			break;
		case 6:
			m_iNextAnimationIndex = OWL_IN_POSITION_BREATH_DEEP;
			break;
		case 7:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_LOSE_HEART;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_LOSE_HEART)
					m_iNextAnimationIndex = OWL_IN_POSITION_LOSE_HEART_KEEP;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_LOSE_HEART_KEEP)
					m_iNextAnimationIndex = OWL_IN_POSITION_LOSE_HEART_RETURN;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_LOSE_HEART_RETURN)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 8:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_GLARE;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_GLARE)
					m_iNextAnimationIndex = OWL_IN_POSITION_GLARE_KEEP;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_GLARE_KEEP)
					m_iNextAnimationIndex = OWL_IN_POSITION_GLARE_RETURN;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_GLARE_RETURN)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 9:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TILT_BACK;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TILT_BACK)
					m_iNextAnimationIndex = OWL_IN_POSITION_TILT_BACK_KEEP;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TILT_BACK_KEEP)
					m_iNextAnimationIndex = OWL_IN_POSITION_TILT_BACK_RETURN;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TILT_BACK_RETURN)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 10:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_DOZE;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_DOZE)
					m_iNextAnimationIndex = OWL_IN_POSITION_DOZE_KEEP;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_DOZE_KEEP)
					m_iNextAnimationIndex = OWL_IN_POSITION_DOZE_RETURN;
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_DOZE_RETURN)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 11:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_KEEP;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_KEEP)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 12:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_BACKWARD;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TRUN_BACKWARD)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 13:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TRUN_SIDE_TO_SIDE)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
					m_iNextAnimationIndex = OWL_IN_POSITION_IDLE;
			}
			break;
		case 14:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TRUN_SIDE_TO_SIDE)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;

				if (m_bInRoop) {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				}
				else {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_POSITION_TO_AWAY_DIAGONALLY;
				}
			}
			break;
		case 15:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TRUN_SIDE_TO_SIDE)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;

				if (m_bInRoop) {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				}
				else {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_POSITION_TO_AWAY_DIAGONALLY_UP;
				}
			}
			break;
		case 16:
			if (m_iCurrentAnimationIndex == OWL_IN_POSITION_IDLE)
				m_iNextAnimationIndex = OWL_IN_POSITION_TO_FLYING;

			// 다음 애니메이션이 끝났을 경우에
			if (m_pModelCom->Get_Animation(m_iNextAnimationIndex)->Get_Finish()) {
				if (m_iCurrentAnimationIndex == OWL_IN_POSITION_TO_FLYING)
					m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TRUN_SIDE_TO_SIDE)
					m_iNextAnimationIndex = OWL_IN_FLYING_TO_POSITION;

				if (m_bInRoop) {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_FLYING_TRUN_SIDE_TO_SIDE;
				}
				else {
					if (m_iCurrentAnimationIndex == OWL_IN_FLYING_TO_POSITION)
						m_iNextAnimationIndex = OWL_IN_POSITION_TO_AWAY_UP;
				}
			}
			break;
		default:
			break;
		}
	}








	RELEASE_INSTANCE(CGameInstance);
}

void COwl::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pInteractionUI)
		m_pInteractionUI->LateTick(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom){
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

#ifdef _DEBUG
		m_pRendererCom->Add_DebugComponent(m_pSphereCom);
#endif // _DEBUG
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT COwl::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint      iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			return E_FAIL;
	}
	
#ifdef _DEBUG
	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT COwl::SetUp_Components(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	OwlDesc Desc = *((OwlDesc*)pArg);

	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_STATIC, Desc.pModelTag, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	// 모델의 이름에 따라 다른 번호 부여
	// m_iOwlNumber에 값 채우기
	// ~~


	/* For.Com_Collider */
	CCollider::COLLIDERDESC         ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.fRadius = 10.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT COwl::SetUp_ConstantTable()
{
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

COwl * COwl::Create(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
{
	COwl*   pInstance = new COwl(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created Owl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COwl::Clone(void * pArg)
{
	COwl*   pInstance = new COwl(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created Clone Owl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COwl::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSphereCom);
}

void COwl::Check_Collision(void)
{
	CCollider* pCollider = dynamic_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_SPHERE_Interaction")));

	if (nullptr == pCollider) {
		MSG_BOX(TEXT("COwl : No Interaction Collider in Player"));
		return;
	}

	// 아직 플레이어와 만나지 않은 경우
	if (!m_bEncounter) {
		// 플레이어의 상호작용 충돌체를 가져와 충돌을 처리
		if (m_pSphereCom->Collision(pCollider)) {
			m_bEncounter = true;
		}
	}
}

void COwl::Check_InRange(void)
{
	CCollider* pCollider = dynamic_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_SPHERE_Interaction")));

	if (nullptr == pCollider) {
		MSG_BOX(TEXT("COwl : No Interaction Collider in Player"));
		return;
	}

	// 아직 플레이어와 만나지 않은 경우
	if (!m_bInRange) {
		// 플레이어의 상호작용 충돌체를 가져와 충돌을 처리
		if (m_pSphereCom->Collision(pCollider)) {
			m_bInRange = true;
		}
	}
}
