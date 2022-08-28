#include "stdafx.h"
#include "..\Public\Lord_Balseph.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Channel.h"
#include "Sword.h"
#include "BalsephAxe.h"
CLord_Balseph::CLord_Balseph(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CEnemy(pDeviceOut, pDeviceContextOut)
{

}

CLord_Balseph::CLord_Balseph(const CLord_Balseph & rhs)
	: CEnemy(rhs)
{
}

HRESULT CLord_Balseph::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLord_Balseph::NativeConstruct(void * pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 1.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 10.f, 1.f));
	m_pModelCom->Set_AnimationIndex(MOVE_IDLE);
	m_pModelFaceCom->Set_AnimationIndex(MOVE_IDLE);
	m_pModelSKL->Set_AnimationIndex(MOVE_IDLE);


	CBalsephAxe::AXEDESC			SocketDesc;
	ZeroMemory(&SocketDesc, sizeof(CBalsephAxe::AXEDESC));

	//SocketDesc.pModelCom = (CModel*)pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Balseph"), TEXT("Com_ModelSKL"));
	SocketDesc.pModelCom = m_pModelSKL;
	SocketDesc.pBoneName = "KK_R";
	SocketDesc.pTargetTransform = m_pTransformCom;
	m_pAxe = dynamic_cast<CBalsephAxe*>(pGameInstance->Clone_Prototype(TEXT("Prototype_GameObject_BalsephAxe"), &SocketDesc));

	if (nullptr == m_pAxe)
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	m_isBoss = true;		// 보스면 이걸 해야 한다. 다른 잡몹들처럼 충돌하면 다른 위치에서 시작하는게 아니라 그 위치에서 바로 시작하도록 하기 하는것이다
	m_bOnce = false;
	m_bStart = true;
	m_bBattle = false;
	
	m_iEnemyInfo.m_iMaxHp = 8;	// 원래 최대 체력 50
	m_iEnemyInfo.m_iHp = m_iEnemyInfo.m_iMaxHp;

	m_eMapType = MAP_BALSELPH;

	return S_OK;
}

void CLord_Balseph::Tick(_double TimeDelta)
{
	if (nullptr != m_pAxe)
		m_pAxe->Tick(TimeDelta);

	m_bOnAttackCollider = false;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector pPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);


	if (!m_bStartScene && m_pTransformCom->Distance(pPos, 10.f))
	{
		m_bStart = true;
		StartScene();
	}

	if ((m_iEnemyInfo.m_iHp > 0 && !m_bAfterColAnim) || m_bAttackRevenge)
	{
		if ((!m_bAttackCollision && m_bStartScene) || m_bAttackRevenge)
		{
			m_TimeDelta += TimeDelta;

			if ((m_TimeDelta > 6.0) && (m_bOnce == false))
			{
				m_bStart = true;

				if (m_iMotion == 7)
					m_iMotion = 0;

				else
					m_iMotion++;
			}

			switch (m_iMotion)
			{
			case 0:
				AttackPattern1(TimeDelta);
				break;

			case 1:
				AttackPattern2(TimeDelta);
				break;

			case 2:
				AttackPattern3(TimeDelta);
				break;

			case 3:
				AttackPattern4(TimeDelta);
				break;

			case 4:
				AttackPattern5(TimeDelta);
				break;

			case 5:
				AttackPattern6(TimeDelta);
				break;

			case 6:
				AttackPattern7(TimeDelta);
				break;

			default:
				return;
			}
		}
	}

	if (m_iCurrentAnimationIndex == m_iNextAnimationIndex)
	{
		m_pModelCom->Update(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_bCutAnimation);
		m_pModelFaceCom->Update(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_bCutAnimation);
		m_pModelSKL->Update(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_bCutAnimation);


		// 이동값 까지 계산해서 애니메이션에 보여준다
		// 여기서부터
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		_vector MoveVector = XMVector3TransformCoord(XMLoadFloat3(&m_pModelCom->Get_CurAnimation()->Get_MoveVector()), m_pModelCom->Get_PivotMatrix());



		_matrix		BoneMatrix = WorldMatrix;

		BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
		BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
		BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);
		BoneMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);


		MoveVector = XMVector3TransformCoord(MoveVector, BoneMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + MoveVector, 1.f));
		// 여기까지 복붙
	}

	else
	{
		m_pModelCom->Update_NextAnim(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
		m_pModelFaceCom->Update_NextAnim(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
		m_pModelSKL->Update_NextAnim(TimeDelta*m_dAnimSpeed* m_dTimeSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
	}

	Compute_Collider();


	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pAttackSphereCom->Update(XMMatrixIdentity());

	__super::Tick(TimeDelta);
	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::LateTick(_double TimeDelta)
{
	if (nullptr != m_pAxe)
		m_pAxe->LateTick(TimeDelta);

	__super::LateTick(TimeDelta);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector pPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (!m_bAttackRevenge)
	{
		if (m_iEnemyInfo.m_iHp > 0 && !m_bAfterColAnim)
		{
			if (m_bAttackCollision)
			{
				m_bHit = true;
				m_bAttackCollision = false;
				m_bCutAnimation = false;


				if (!(m_pTransformCom->FrontPlayer(pPos)))
				{
					//m_iDuration = 10.0;
					m_bCutAnimation = false;
					m_iNextAnimationIndex = BTL_DOWN_F;
					m_bOnce = false;
					//HitPattern1(TimeDelta);
				}

				else
				{
					m_bCutAnimation = false;
					m_iNextAnimationIndex = DOWN_UNIQUE;
					m_bOnce = false;
					//HitPattern2(TimeDelta);
				}

			}
		}
	}


	if (m_iCurrentAnimationIndex == BTL_DOWN_F)
	{
		m_bAfterColAnim = true;
		if (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1)
		{
			m_iDuration = 10.0;
			m_bCutAnimation = false;
			m_iNextAnimationIndex = ARISE_F;
			m_bAfterColAnim = false;
		}
	}

	else if (m_iCurrentAnimationIndex == DOWN_UNIQUE || m_iCurrentAnimationIndex == ARISE_F)
	{
		m_bAfterColAnim = true;
		if (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1)
		{
			m_iDuration = 10.0;
			m_bCutAnimation = true;
			m_iNextAnimationIndex = MOVE_IDLE;
			m_bAfterColAnim = false;
		}
	}


	if (m_iEnemyInfo.m_iHp <= 0)
	{
		m_bHit = false;
		m_bCutAnimation = false;
		m_dAnimSpeed = 2.0;
		m_iNextAnimationIndex = BTL_DEAD;
		m_bDeadOnce = true;
	}

	if (m_bDeadOnce)
	{
		m_dDeathTime += TimeDelta;
	}

	if ((m_iCurrentAnimationIndex == BTL_DEAD) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		if (m_dDeathTime > 7.f)
			m_bDead = true;
	}

	///////////////////////////////////////////////////

	if (m_iCurrentAnimationIndex != BTL_DEAD)
	{
		if (m_bAfterColAnim)			// 몬스터가 계속 맞기만 하면 공격을 할 수 없기에 반격할 기회를 주기위해 만든 함수
		{
			m_dColTime += TimeDelta;
			if (m_dColTime > 4.f)
			{
				m_bAfterColAnim = false;
				m_bAttackRevenge = true;
			}
		}

		else
		{
			m_dColTime = 0.0;
		}
	}
	/*if (!m_bAttackRevenge)
	{
	if (m_iEnemyInfo.m_iHp > 0 && !m_bAfterColAnim)
	{
	m_TimeDelta += TimeDelta;

	if ((m_TimeDelta > 5.0) && (m_bOnce == false))
	{
	m_bStart = true;

	if (m_iHitMotion == 2)
	m_iHitMotion = 0;

	else
	m_iHitMotion++;
	}


	switch (m_iHitMotion)
	{
	case 0:
	{
	if (m_pTransformCom->FrontPlayer(pPos))
	HitPattern1(TimeDelta);

	break;
	}

	case 1:
	{
	if (!(m_pTransformCom->FrontPlayer(pPos)))
	HitPattern2(TimeDelta);

	break;
	}


	default:
	return;
	}
	}
	}*/
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLord_Balseph::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;


	_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			return E_FAIL;

	}

	_uint		iNumFaceMeshContainers = m_pModelFaceCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumFaceMeshContainers; ++i)
	{
		if (FAILED(m_pModelFaceCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelFaceCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelFaceCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			return E_FAIL;

	}

	if (m_bOnAttackCollider)
	{
		m_pAttackSphereCom->Render();
	}

	return S_OK;
}


HRESULT CLord_Balseph::SetUp_Components()
{
	///* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_LORD_BALSEPH, TEXT("Prototype_Component_Model_Balseph"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_ModelFace"), LEVEL_LORD_BALSEPH, TEXT("Prototype_Component_Model_BalsephFace"), (CComponent**)&m_pModelFaceCom)))
		return E_FAIL;

	///* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_ModelSKL"), LEVEL_LORD_BALSEPH, TEXT("Prototype_Component_Model_BalsephSKL"), (CComponent**)&m_pModelSKL)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_OBB */
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_AttackSPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pAttackSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CLord_Balseph::AttackPattern1(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//인중이가 알려준 코드
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_LOOP) && (0.08f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.07f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}

	else if (m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_LOOP)
	{
		m_pTransformCom->Look(NPos);
	}


	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_iNextAnimationIndex = ATTACK_MOUNTAIN_BUSTER_START;
		m_bCutAnimation = false;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_START) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = ATTACK_MOUNTAIN_BUSTER_LOOP;
	}

	else if (m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_LOOP)
	{
		if (!m_pTransformCom->Distance(vPlayerPos, 4.f))
		{
			m_bCutAnimation = false;
			if (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1)
			{
				m_pModelCom->Get_CurAnimation()->Reset_Animation();
				m_pModelFaceCom->Get_CurAnimation()->Reset_Animation();
				m_pModelSKL->Get_CurAnimation()->Reset_Animation();
			}
		}

		else
		{
			m_bCutAnimation = false;
			m_iDuration = 2.0;
			m_dAnimSpeed = 3.0;
			m_iNextAnimationIndex = ATTACK_MOUNTAIN_BUSTER_END;
		}
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_END) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1)))
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == ATTACK_MOUNTAIN_BUSTER_END)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 45) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 50))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*4.f + XMVectorSet(0.f, 1.5f, 0.f, 0.f), 1.5f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_MOUNTAIN_BUSTER_LOOP)->Get_TimeAcc()) == 1 || ((_uint)m_pModelCom->Get_Animation(ATTACK_MOUNTAIN_BUSTER_LOOP)->Get_TimeAcc()) == 3 || ((_uint)m_pModelCom->Get_Animation(ATTACK_MOUNTAIN_BUSTER_LOOP)->Get_TimeAcc()) == 10)
	{
		CRect_Effect* Effect = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_MONSTER, 3));
		Effect->Set_Transform(UNIT_MONSTER, this, m_pTransformCom, _float3(1.7f, 0.f, -0.5f));
		(Effect->Get_Transfrom())->Scaled(_float3(0.1f, 1.f, 0.1f));
		Effect->Set_TimeSpeed(9.f);
		/*CMeshEffect* Effect0 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 28));
		Effect0->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect0->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect0->Get_Transfrom())->GO_RUL(_float3(1.5f, 0.f, 0.f));
		Effect0->Set_Finish_Dead(false);
		(Effect0->Get_Transfrom())->Turn_Angle((Effect0->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
		Effect0->Set_TimeSpeed(1.f);*/
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_MOUNTAIN_BUSTER_END)->Get_TimeAcc()) == 40)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 10));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(1.7f, 0.f, 0.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(-2.f));

		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 28));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(20.f, 20.f, 20.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(1.7f, 0.f, 0.f));
		(Effect1->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(-2.f));
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern2(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_POWER_WAVE;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_POWER_WAVE) &&
		(m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 80)
		&& m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 83)
	{
		CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
		if (nullptr == pPlayerTransform)
			return;

		_vector pPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->LookAt(pPos);
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_POWER_WAVE) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1)))
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == ATTACK_POWER_WAVE)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 88) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 135))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_POWER_WAVE)->Get_TimeAcc()) == 82)
	{
		CMeshEffect* Effect0 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 21));
		Effect0->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect0->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect0->Get_Transfrom())->GO_RUL(_float3(0.f, 0.2f, 0.f));
		Effect0->Set_TimeSpeed(1.3f);
		(Effect0->Get_Transfrom())->Turn_Angle((Effect0->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(50.f));


		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 21));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(0.f, 0.2f, 0.f));
		Effect1->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 17));
		Effect2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect2->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect2->Get_Transfrom())->GO_RUL(_float3(0.f, 0.1f, 0.f));
		Effect2->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect3 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 18));
		Effect3->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect3->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect3->Get_Transfrom())->GO_RUL(_float3(0.f, 0.1f, 0.f));
		Effect3->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect4 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 19));
		Effect4->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect4->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect4->Get_Transfrom())->GO_RUL(_float3(0.f, 0.1f, 0.f));
		Effect4->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect5 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 20));
		Effect5->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect5->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect5->Get_Transfrom())->GO_RUL(_float3(0.f, 0.2f, 0.f));
		Effect5->Set_TimeSpeed(1.3f);
		(Effect5->Get_Transfrom())->Turn_Angle((Effect5->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	}


	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern3(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//인중이가 알려준 코드
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == ATTACK_JUMP_SAVE_START) && (0.05f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.04f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == ATTACK_JUMP_SAVE_START)
	{

		m_pTransformCom->Look(NPos);
	}



	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_JUMP_SAVE_START;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_JUMP_SAVE_START) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = ATTACK_JUMP_SAVE_LOOP;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_JUMP_SAVE_LOOP) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 3.0;
		m_iNextAnimationIndex = ATTACK_JUMP_CRUSH;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_JUMP_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1)))
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}


	if (m_iCurrentAnimationIndex == ATTACK_JUMP_CRUSH)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 75) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 95))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*4.5f, 3.f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_JUMP_CRUSH)->Get_TimeAcc()) == 75)
	{
		CMeshEffect* Effect0 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 22));
		Effect0->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect0->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect0->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect0->Set_TimeSpeed(1.3f);


		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 23));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(4.f, 5.f, 4.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 5.5f));
		Effect1->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 18));
		Effect2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect2->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect2->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect2->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect3 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 19));
		Effect3->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect3->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect3->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect3->Set_TimeSpeed(1.3f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern4(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//인중이가 알려준 코드
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == ATTACK_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 70) && (0.21f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.1f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}


	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.2;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_CRUSH;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 111))
	{
		m_iDuration = 1.0;
		m_dAnimSpeed = 3.2;
		m_iNextAnimationIndex = ATTACK_SWING;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_SWING) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == ATTACK_CRUSH)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 87) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 120))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*4.5f, 3.f);
		}
	}

	else if (m_iCurrentAnimationIndex == ATTACK_SWING)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 70) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 105))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*2.f, 3.f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_CRUSH)->Get_TimeAcc()) == 88)
	{
		CMeshEffect* Effect0 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 22));
		Effect0->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect0->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect0->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect0->Set_TimeSpeed(1.3f);


		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 23));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(4.f, 5.f, 4.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 5.5f));
		Effect1->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 18));
		Effect2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect2->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect2->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect2->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect3 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 19));
		Effect3->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect3->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect3->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect3->Set_TimeSpeed(1.3f);
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_SWING)->Get_TimeAcc()) == 70)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 25));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(4.f, 3.f, 4.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 1.5f, 0.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(1.f));
		Effect->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 26));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(4.f, 3.f, 4.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(0.f, 1.5f, 0.f));
		(Effect1->Get_Transfrom())->Turn_Angle((Effect1->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(1.f));
		Effect1->Set_TimeSpeed(1.3f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern5(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == ATTACK_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 70) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 93) && (0.15f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.14f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}

	//if ((m_iCurrentAnimationIndex == ATTACK_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 70) &&
	//	(m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 71))
	//{
	//	m_pTransformCom->LookAt(vPlayerPos);
	//}

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 2.5;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_CRUSH;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_CRUSH) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == ATTACK_CRUSH)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 87) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 120))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*5.f, 1.5f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_CRUSH)->Get_TimeAcc()) == 88)
	{
		CMeshEffect* Effect0 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 22));
		Effect0->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect0->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect0->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect0->Set_TimeSpeed(1.3f);


		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 23));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(4.f, 5.f, 4.f));
		(Effect1->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 5.5f));
		Effect1->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 18));
		Effect2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect2->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect2->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect2->Set_TimeSpeed(1.3f);

		CMeshEffect* Effect3 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 19));
		Effect3->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect3->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect3->Get_Transfrom())->GO_RUL(_float3(0.f, 0.0f, 5.5f));
		Effect3->Set_TimeSpeed(1.3f);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern6(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 1.9;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_JUMP_PRESS;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_JUMP_PRESS) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == ATTACK_JUMP_PRESS)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 66) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 90))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.5f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(ATTACK_JUMP_PRESS)->Get_TimeAcc()) == 68)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 18));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 2.f, 3.f));

		CMeshEffect* Effect1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 8));
		Effect1->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect1->Get_Transfrom())->Scaled(_float3(4.f, 0.3f, 4.f));
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::AttackPattern7(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//인중이가 알려준 코드
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == MASTER_CORE_START) && (0.04f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.03f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == MASTER_CORE_START)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = MASTER_CORE_START;
	}

	else if ((m_iCurrentAnimationIndex == MASTER_CORE_START) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_makeCoreTime = 0.0;
		m_iNextAnimationIndex = MASTER_CORE_LOOP;
	}

	else if (m_iCurrentAnimationIndex == MASTER_CORE_LOOP)
	{
		m_makeCoreTime += TimeDelta;

		if (m_makeCoreTime < 3.0)
		{
			if (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1)
			{
				m_pModelCom->Get_CurAnimation()->Reset_Animation();
				m_pModelFaceCom->Get_CurAnimation()->Reset_Animation();
				m_pModelSKL->Get_CurAnimation()->Reset_Animation();
			}
		}

		else if ((m_iCurrentAnimationIndex == MASTER_CORE_LOOP) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
		{
			m_iDuration = 10.0;
			m_dAnimSpeed = 2.0;
			m_bCutAnimation = true;
			m_iNextAnimationIndex = MOVE_IDLE;
			m_bOnce = false;
			m_TimeDelta = 0.0;
			m_bAttackRevenge = false;
		}
	}


	if (m_iCurrentAnimationIndex == MASTER_CORE_LOOP)
	{
		m_bOnAttackCollider = true;
		m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f);
	}


	if (((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 1 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 20 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 40 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 60 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 80 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 100)

	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 30));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.2f, 1.7f, 1.f));
		Effect->Set_TimeSpeed(1.f);
	}

	if (((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 10 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 30 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 50 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 70 ||
		((_uint)m_pModelCom->Get_Animation(MASTER_CORE_LOOP)->Get_TimeAcc()) == 90)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 30));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(15.f, 15.f, 15.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.2f, 1.7f, 1.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(30.f));
		Effect->Set_TimeSpeed(1.5f);
	}
	RELEASE_INSTANCE(CGameInstance);
}


void CLord_Balseph::HitPattern1(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = DOWN_UNIQUE;
	}

	else if ((m_iCurrentAnimationIndex == DOWN_UNIQUE) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::HitPattern2(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = BTL_DOWN_F;
	}

	else if ((m_iCurrentAnimationIndex == BTL_DOWN_F) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = ARISE_F;
	}

	else if ((m_iCurrentAnimationIndex == ARISE_F) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::MovePattern()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//인중이가 알려준 코드
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector Pos = vPlayerPos - vPos;
	_vector NLook = XMVector3Normalize(vLook);
	_vector NPos = XMVector3Normalize(Pos);
	_vector Look = NLook - NPos;


	if ((m_iCurrentAnimationIndex == MOVE_WALK_F) && (0.21f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.2f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == MOVE_WALK_F)
	{
		m_pTransformCom->Look(NPos);
		m_iNextAnimationIndex = MOVE_IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::StartScene()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_TRUN180;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_TRUN180) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bStartScene = true;
		m_pBattle_Manager->Battle_Enter(this);
		m_bDead = true;
		//m_pTransformCom->Set_State(Ctransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		//모션 끝나면 위치 세팅 해줘야 한다.
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::TurnLeft(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 1.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = TURN_L;
	}

	else if ((m_iCurrentAnimationIndex == TURN_L) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{

		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CLord_Balseph::TurnRight(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);



	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_dAnimSpeed = 1.5;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = TURN_R;
	}

	else if ((m_iCurrentAnimationIndex == TURN_R) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CLord_Balseph::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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


CLord_Balseph * CLord_Balseph::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CLord_Balseph*	pInstance = new CLord_Balseph(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CLord_Balseph"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLord_Balseph::Clone(void * pArg)
{
	CLord_Balseph*	pInstance = new CLord_Balseph(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created Clone CLord_Balseph"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLord_Balseph::Free()
{
	Safe_Release(m_pModelSKL);
	Safe_Release(m_pModelFaceCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pAxe);

	__super::Free();
}