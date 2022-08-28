#include "stdafx.h"
#include "..\Public\Punisher.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Channel.h"
#include "Sword.h"
CPunisher::CPunisher(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CEnemy(pDeviceOut, pDeviceContextOut)
{

}

CPunisher::CPunisher(const CPunisher & rhs)
	: CEnemy(rhs)
{
}

HRESULT CPunisher::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPunisher::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(MOVE_IDLE);

	m_isBoss = true;
	m_bOnce = false;
	m_bStart = true;
	m_bBattle = false;
	m_iEnemyInfo.m_iHp = 50;
	return S_OK;
}

void CPunisher::Tick(_double TimeDelta)
{
	m_bOnAttackCollider = false;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix BoneMatrix0, BoneMatrix1, BoneMatrix2, BoneMatrix3, BoneMatrix4, BoneMatrix5;



	for (_int i = 0; i < 6; ++i)
	{
		if (HE[i] != nullptr)
		{
			_float4x4 m_PivotMatrix = m_pModelCom->Get_PivotMatrix4x4();

			switch (i)
			{

			case 0:
				BoneMatrix0 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND_R"));

				BoneMatrix0.r[0] = XMVector3Normalize(BoneMatrix0.r[0]);
				BoneMatrix0.r[1] = XMVector3Normalize(BoneMatrix0.r[1]);
				BoneMatrix0.r[2] = XMVector3Normalize(BoneMatrix0.r[2]);

				HE[0]->Set_ParentsMatrix(BoneMatrix0 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[0]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 1.f));
				(HE[0]->Get_Transfrom())->Turn_Angle(HE[0]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(180.f));
				(HE[0]->Get_Transfrom())->Turn_Angle(HE[0]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-90.f));


				//(HE[0]->Get_Transfrom())->Turn_Angle(HE[0]->Get_Transfrom()->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));
				break;

			case 1:
				BoneMatrix1 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND2_R"));

				BoneMatrix1.r[0] = XMVector3Normalize(BoneMatrix1.r[0]);
				BoneMatrix1.r[1] = XMVector3Normalize(BoneMatrix1.r[1]);
				BoneMatrix1.r[2] = XMVector3Normalize(BoneMatrix1.r[2]);

				HE[1]->Set_ParentsMatrix(BoneMatrix1 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[1]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));
				(HE[1]->Get_Transfrom())->Turn_Angle(HE[1]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(180.f));
				(HE[1]->Get_Transfrom())->Turn_Angle(HE[1]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
				break;


			case 2:
				BoneMatrix2 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND3_R"));

				BoneMatrix2.r[0] = XMVector3Normalize(BoneMatrix2.r[0]);
				BoneMatrix2.r[1] = XMVector3Normalize(BoneMatrix2.r[1]);
				BoneMatrix2.r[2] = XMVector3Normalize(BoneMatrix2.r[2]);

				HE[2]->Set_ParentsMatrix(BoneMatrix2 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[2]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));
				(HE[2]->Get_Transfrom())->Turn_Angle(HE[2]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
				(HE[2]->Get_Transfrom())->Turn_Angle(HE[2]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
				(HE[2]->Get_Transfrom())->Turn_Angle(HE[2]->Get_Transfrom()->Get_State(CTransform::STATE_UP), XMConvertToRadians(170.f));
				break;


			case 3:
				BoneMatrix3 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND3_L"));

				BoneMatrix3.r[0] = XMVector3Normalize(BoneMatrix3.r[0]);
				BoneMatrix3.r[1] = XMVector3Normalize(BoneMatrix3.r[1]);
				BoneMatrix3.r[2] = XMVector3Normalize(BoneMatrix3.r[2]);

				HE[3]->Set_ParentsMatrix(BoneMatrix3 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[3]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 1.3f));
				(HE[3]->Get_Transfrom())->Turn_Angle(HE[3]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
				(HE[3]->Get_Transfrom())->Turn_Angle(HE[3]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(270.f));
				(HE[3]->Get_Transfrom())->Turn_Angle(HE[3]->Get_Transfrom()->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));
				break;

			case 4:
				BoneMatrix4 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND2_L"));

				BoneMatrix4.r[0] = XMVector3Normalize(BoneMatrix4.r[0]);
				BoneMatrix4.r[1] = XMVector3Normalize(BoneMatrix4.r[1]);
				BoneMatrix4.r[2] = XMVector3Normalize(BoneMatrix4.r[2]);

				HE[4]->Set_ParentsMatrix(BoneMatrix4 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[4]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 1.3f));
				(HE[4]->Get_Transfrom())->Turn_Angle(HE[4]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
				(HE[4]->Get_Transfrom())->Turn_Angle(HE[4]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(270.f));

				break;

			case 5:
				BoneMatrix5 = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("HAND_L"));

				BoneMatrix5.r[0] = XMVector3Normalize(BoneMatrix5.r[0]);
				BoneMatrix5.r[1] = XMVector3Normalize(BoneMatrix5.r[1]);
				BoneMatrix5.r[2] = XMVector3Normalize(BoneMatrix5.r[2]);

				HE[5]->Set_ParentsMatrix(BoneMatrix5 * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix());

				(HE[5]->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, 1.f));
				(HE[5]->Get_Transfrom())->Turn_Angle(HE[5]->Get_Transfrom()->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
				(HE[5]->Get_Transfrom())->Turn_Angle(HE[5]->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(270.f));
				break;
			}


			(HE[i]->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));


			if (HE[i]->Get_Finish())
			{
				HE[i]->Set_Dead(true);
				HE[i] = nullptr;
			}
		}
	}



	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector pPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);


	if (!m_bBattle)  // 전투중이 아니라면 하는 행동패턴
	{
		m_TimeDelta += TimeDelta;

		if (m_TimeDelta > 5.0)
		{
			m_bStart = true;

			if (m_pTransformCom->Distance(pPlayerPos, 4.f))
				NotBattleMode1();

			else
				NotBattleMode2();
		}
	}

	if (!m_bStartScene && m_bBattle)
	{
		m_bStart = true;
		StartScene();
	}


	if ((m_iEnemyInfo.m_iHp > 0 && !m_bAfterColAnim) || m_bAttackRevenge)
	{
		if ((!m_bAttackCollision && m_bStartScene && m_bBattle) || m_bAttackRevenge)		// 플레이어가 공격을 하지 않는다면 동작 개시
		{
			m_TimeDelta += TimeDelta;

			if ((m_TimeDelta > 5.0) && (m_bOnce == false))
			{

				m_bStart = true;

//				m_iMotion = 2;

				if (m_iMotion == 8)
					m_iMotion = 0;

				else
					m_iMotion++;
			}

			switch (m_iMotion)
			{
			case 0:
				AttackPattern1();
				break;

			case 1:
				AttackPattern2();
				break;

			case 2:
				AttackPattern3(TimeDelta);
				break;

			case 3:
				AttackPattern4();
				break;

			case 4:
				AttackPattern5();
				break;

			case 5:
				AttackPattern6();
				break;

			case 6:
				AttackPattern7();
				break;

			case 7:
				NothingAnim1();
				break;

			default:
				return;
			}
		}
	}

	if (m_iCurrentAnimationIndex == m_iNextAnimationIndex)
	{
		m_pModelCom->Monster_Update(TimeDelta*m_dAnimSpeed * m_dTimeSpeed, m_bCutAnimation);

		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		_vector MoveVector = XMVector3TransformCoord(XMLoadFloat3(&m_pModelCom->Get_CurAnimation()->Get_MoveVector()), m_pModelCom->Get_PivotMatrix());

		_matrix		BoneMatrix = WorldMatrix;

		BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
		BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
		BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);
		BoneMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);


		MoveVector = XMVector3TransformCoord(MoveVector, BoneMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + MoveVector, 1.f));
	}

	else
	{
		m_pModelCom->Monster_Update_NextAnim(TimeDelta*m_dAnimSpeed*m_dTimeSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
	}

	//_matrix      Weapon1BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("weapon_01"));  //대소문자 구별해야함... Weapon 하면터짐
	//_matrix      Weapon2BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedTransformationMatrix("weapon_02"));
	//Weapon1BoneMatrix.r[0] = XMVector3Normalize(Weapon1BoneMatrix.r[0]);
	//Weapon1BoneMatrix.r[1] = XMVector3Normalize(Weapon1BoneMatrix.r[1]);
	//Weapon1BoneMatrix.r[2] = XMVector3Normalize(Weapon1BoneMatrix.r[2]);

	//Weapon2BoneMatrix.r[0] = XMVector3Normalize(Weapon2BoneMatrix.r[0]);
	//Weapon2BoneMatrix.r[1] = XMVector3Normalize(Weapon2BoneMatrix.r[1]);
	//Weapon2BoneMatrix.r[2] = XMVector3Normalize(Weapon2BoneMatrix.r[2]);

	//Weapon1BoneMatrix = Weapon1BoneMatrix*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix());
	//Weapon2BoneMatrix = Weapon2BoneMatrix*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix());



	//m_pWeapon1AttackSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	//m_pWeapon2AttackSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	//_float4x4 Weapon1Matrix, Weapon2Matrix;
	//XMStoreFloat4x4(&Weapon1Matrix, Weapon1BoneMatrix);
	//XMStoreFloat4x4(&Weapon2Matrix, Weapon2BoneMatrix);



	Compute_Collider();

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pAttackSphereCom->Update(XMMatrixIdentity());

	__super::Tick(TimeDelta);
	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);


	if (!m_bAttackRevenge)
	{
		if (m_iEnemyInfo.m_iHp > 0 && !m_bAfterColAnim)
		{
			if (m_bAttackCollision && m_bBattle)
			{
				//if (m_bHit)
				//{
				//	m_pModelCom->Get_CurAnimation()->Reset_MonsterAnimation();
				//}

				m_bHit = true;
				m_bAttackCollision = false;
				m_bCutAnimation = false;

				if (!(m_pTransformCom->FrontPlayer(vPos)))
				{
					//m_bAfterColAnim = true;
					m_bCutAnimation = false;
					m_iNextAnimationIndex = DOWN_B;
					m_bOnce = false;
				}

				else
				{
					//m_bAfterColAnim = true;
					m_bCutAnimation = false;
					m_iNextAnimationIndex = DOWN_UNIQUE;
					m_bOnce = false;
				}
			}
		}
	}

	if (m_iCurrentAnimationIndex == DOWN_B || m_iCurrentAnimationIndex == DOWN_UNIQUE)
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
		m_iNextAnimationIndex = DEAD;
		m_bDeadOnce = true;
	}

	if (m_bDeadOnce)
	{
		m_dDeathTime += TimeDelta;
	}

	if ((m_iCurrentAnimationIndex == DEAD) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		if (m_dDeathTime > 7.f)
			m_bDead = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	if (m_iCurrentAnimationIndex != DEAD)
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


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPunisher::Render()
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

	if (m_bOnAttackCollider)
	{
		m_pAttackSphereCom->Render();
	}

	return S_OK;
}

void CPunisher::AttackPattern1()
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

	if ((m_iCurrentAnimationIndex == IN_OUT_UPPER) && (0.10f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.08f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == IN_OUT_UPPER)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = IN_OUT_UPPER;
	}

	else if (m_iCurrentAnimationIndex == IN_OUT_UPPER)
	{

		if (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1)
		{
			m_dAnimSpeed = 2.0;
			m_bCutAnimation = true;
			m_iNextAnimationIndex = MOVE_IDLE;
			m_bOnce = false;
			m_TimeDelta = 0.0;
			m_bAttackRevenge = false;

		}


		if ((m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 64 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 90))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 1.f, 0.f, 0.f), 1.8f);
		}



		else if (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 190 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 215)
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 3.f, 0.f, 0.f), 1.8f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(IN_OUT_UPPER)->Get_TimeAcc()) == 66)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 14));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 2.3f, 0.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(230.f));

		Effect->Set_TimeSpeed(2.5f);
	}

	if (((_uint)m_pModelCom->Get_Animation(IN_OUT_UPPER)->Get_TimeAcc()) == 194)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 14));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(-2.f, 2.3f, 0.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(150.f));

		Effect->Set_TimeSpeed(2.5f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern2()
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


	if ((m_iCurrentAnimationIndex == MULTI) && (0.12f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.11f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == MULTI)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = MULTI;
	}


	else if (m_iCurrentAnimationIndex == MULTI)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1)
		{
			m_dAnimSpeed = 2.0;
			m_bCutAnimation = true;
			m_iNextAnimationIndex = MOVE_IDLE;
			m_bOnce = false;
			m_TimeDelta = 0.0;
			m_bAttackRevenge = false;
		}


		if ((m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 100 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 115)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 123 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 135)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 148 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 156)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 168 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 173)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 187 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 193)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 212 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 220))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 4.5f, 1.8f);
		}

		else if ((m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 280 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 310))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*4.5f, 1.f);
		}

	}

	if (((_uint)m_pModelCom->Get_Animation(MULTI)->Get_TimeAcc()) == 2)
	{

		for (int i = 0; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 11));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(1.2f);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern3(_double TimeDelta)
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


	if ((m_iCurrentAnimationIndex == RUSH_LOOP) && (0.11f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.1f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}

	else if (m_iCurrentAnimationIndex == RUSH_LOOP)
	{
		m_pTransformCom->Look(NPos);
	}


	if (m_iCurrentAnimationIndex == RUSH_LOOP)		// 콜라이더 생성용 조건문
	{
		m_bOnAttackCollider = true;
		m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK), 1.8f);
	}


	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 3.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = RUSH_START;
	}

	else if ((m_iCurrentAnimationIndex == RUSH_START) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = RUSH_LOOP;
	}

	else if (m_iCurrentAnimationIndex == RUSH_LOOP)
	{
		//m_pTransformCom->LookAt(vPlayerPos);
		if (!m_pTransformCom->Distance(vPlayerPos, 3.f))
		{
			m_bCutAnimation = false;
			if (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1)
			{
				m_pModelCom->Get_CurAnimation()->Reset_MonsterAnimation();
			}
		}

		else
		{
			m_iDuration = 5.0;
			m_dAnimSpeed = 2.0;
			m_bCutAnimation = true;
			m_iNextAnimationIndex = MOVE_IDLE;
			m_bOnce = false;
			m_TimeDelta = 0.0;
			m_bAttackRevenge = false;
			for (int i = 0; i < 6; ++i)
			{
				HE[i]->Set_TimeSpeed(2.f);
			}
		}
	}


	if (((_uint)m_pModelCom->Get_Animation(RUSH_START)->Get_TimeAcc()) == 123)
	{
		for (int i = 0; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 12));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(1.f);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern4()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_iCurrentAnimationIndex == HANDSTAND_FOOTPRESS)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 150 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 290)
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK), 4.f);
		}
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = HANDSTAND_FOOTPRESS;
	}

	else if ((m_iCurrentAnimationIndex == HANDSTAND_FOOTPRESS) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (((_uint)m_pModelCom->Get_Animation(HANDSTAND_FOOTPRESS)->Get_TimeAcc()) == 2)
	{
		for (int i = 0; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 11));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(0.9f);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern5()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = SWING_START;
		m_iAnimCount = 0;
	}

	else if ((m_iCurrentAnimationIndex == SWING_START) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = SWING_LOOP;
	}

	else if ((m_iCurrentAnimationIndex == SWING_LOOP) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		if (m_iAnimCount != 3)
		{
			m_pModelCom->Get_CurAnimation()->Reset_MonsterAnimation();
			m_iAnimCount += 1;
		}
		else
			m_iNextAnimationIndex = SWING_END;
	}

	else if ((m_iCurrentAnimationIndex == SWING_END) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}


	if (m_iCurrentAnimationIndex == SWING_START)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 178 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 181)
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK), 5.f);
		}
	}

	else if (m_iCurrentAnimationIndex == SWING_LOOP)
	{
		if ((m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 23 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 29)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 39 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 45)
			|| (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 50 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 54))
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 3.f);
		}

	}


	if (((_uint)m_pModelCom->Get_Animation(SWING_START)->Get_TimeAcc()) == 98)
	{
		for (int i = 0; i < 3; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 12));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(1.f);
		}

		for (int i = 3; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 11));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(1.f);
		}
	}


	if (((_uint)m_pModelCom->Get_Animation(SWING_LOOP)->Get_TimeAcc()) == 22)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 15));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 5.f, 2.5f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(45.f));
		//(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(-45.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
		Effect->Set_TimeSpeed(3.f);
	}

	if (((_uint)m_pModelCom->Get_Animation(SWING_LOOP)->Get_TimeAcc()) == 35)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 15));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(45.f));
		//(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(-45.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));

		Effect->Set_TimeSpeed(3.f);
	}

	if (((_uint)m_pModelCom->Get_Animation(SWING_LOOP)->Get_TimeAcc()) == 48)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 15));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 5.f, 6.5f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(45.f));
		//(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_UP), XMConvertToRadians(-45.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
		Effect->Set_TimeSpeed(3.f);
	}



	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern6()
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


	if ((m_iCurrentAnimationIndex == SWING_360) && (0.11f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.1f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == SWING_360)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = SWING_360;
		m_bEffectOnlyOnce = true;
	}

	else if ((m_iCurrentAnimationIndex == SWING_360) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == SWING_360)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 127 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 135)
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f);
		}
	}

	if ((((_uint)m_pModelCom->Get_Animation(SWING_360)->Get_TimeAcc()) == 65) && m_bEffectOnlyOnce)
	{
		for (int i = 0; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 12));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(2.f);
		}

		m_bEffectOnlyOnce = false;
	}

	if (((_uint)m_pModelCom->Get_Animation(SWING_360)->Get_TimeAcc()) == 119)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 33));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 2.3f, 3.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(50.f));

		Effect->Set_TimeSpeed(2.5f);
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::AttackPattern7()
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


	if ((m_iCurrentAnimationIndex == SWORD_720_FIRST) && (0.11f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.1f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == SWORD_720_FIRST)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.5;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = SWORD_720_FIRST;
	}

	else if ((m_iCurrentAnimationIndex == SWORD_720_FIRST) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bAttackRevenge = false;
	}

	if (m_iCurrentAnimationIndex == SWORD_720_FIRST)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() >= 105 && m_pModelCom->Get_CurAnimation()->Get_CenterChannel()->Get_CurrentKeyFrameIndex() <= 150)
		{
			m_bOnAttackCollider = true;
			m_pAttackSphereCom->Set(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 3.f, 0.f, 0.f), 5.f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(SWORD_720_FIRST)->Get_TimeAcc()) == 65)
	{
		for (int i = 0; i < 6; ++i)
		{
			HE[i] = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 12));
			HE[i]->Set_Finish_Dead(false);
			HE[i]->Set_TimeSpeed(2.f);
		}
	}

	if (((_uint)m_pModelCom->Get_Animation(SWORD_720_FIRST)->Get_TimeAcc()) == 100)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Monster_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_MONSTER, 35));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(3.f, 3.f, 3.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 2.3f, 3.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(25.f));

		Effect->Set_TimeSpeed(3.5f);
	}

	RELEASE_INSTANCE(CGameInstance);
}



void CPunisher::NothingAnim1()
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


	if ((m_iCurrentAnimationIndex == BRAVE) && (0.16f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.15f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == BRAVE)
	{
		m_pTransformCom->Look(NPos);
	}

	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = BRAVE;
	}

	else if ((m_iCurrentAnimationIndex == BRAVE) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::HitPattern1()
{
	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = DOWN_B;
	}

	else if ((m_iCurrentAnimationIndex == DOWN_B) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}
}

void CPunisher::HitPattern2()
{
	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = DOWN_UNIQUE;
	}

	else if ((m_iCurrentAnimationIndex == DOWN_UNIQUE) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}
}

void CPunisher::NotBattleMode1()
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


	if ((m_iCurrentAnimationIndex == MOVE_IDLE) && (0.12f < XMVectorGetX(XMVector3Length(Look))))
	{
		_vector LookF = XMVector3Normalize(Look) * 0.11f;
		vLook = XMVector3Normalize(vLook) - LookF;
		m_pTransformCom->Look(vLook);
	}
	else if (m_iCurrentAnimationIndex == MOVE_IDLE)
	{
		m_pTransformCom->Look(NPos);
	}

	if (m_iCurrentAnimationIndex == LOOKOUT)
	{
		m_bOnce = false;
		m_bStart = true;
	}


	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 1.0;
		m_iDuration = 10.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = IDLE;
	}

	else if ((m_iCurrentAnimationIndex == IDLE) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPunisher::NotBattleMode2()
{
	if (m_iCurrentAnimationIndex == IDLE)
	{
		m_bOnce = false;
		m_bStart = true;
	}

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 1.0;
		m_iDuration = 10.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = LOOKOUT;
	}

	else if ((m_iCurrentAnimationIndex == LOOKOUT) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}
}

void CPunisher::StartScene()
{
	if (!m_bOnce && m_bStart)
	{
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = SYMBOL_DETECT_IDLE;
	}

	else if ((m_iCurrentAnimationIndex == SYMBOL_DETECT_IDLE) && (m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_MainChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
		m_bStartScene = true;
	}

}



HRESULT CPunisher::SetUp_Components()
{
	///* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_TUTORIAL, TEXT("Prototype_Component_Model_Punisher"), (CComponent**)&m_pModelCom)))
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
	ColliderDesc.vPosition = _float3(0.f, 3.f, 0.f);
	ColliderDesc.fRadius = 3.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_AttackSPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pAttackSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPunisher::SetUp_ConstantTable()
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

CPunisher * CPunisher::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CPunisher*	pInstance = new CPunisher(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CPunisher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunisher::Clone(void * pArg)
{
	CPunisher*	pInstance = new CPunisher(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created Clone CPunisher"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPunisher::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
}

//void CPunisher::Compute_Collider()
//{
//	m_pAttackSphereCom->Set_On(m_bOnAttackCollider);
//}
