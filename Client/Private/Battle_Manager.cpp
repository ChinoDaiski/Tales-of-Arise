#include "stdafx.h"
#include "..\Public\Battle_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Enemy.h"
#include "Player_Manager.h"
#include "Player.h"

#include "PlayerMovingHP.h"
#include"Alphen.h"
#include"Shionne.h"


#include "WayPoint.h"
#include "MapObject.h"
#include "Navigation.h"

#include "Balseph_Stair.h"
#include "Lord_Balseph.h"
#include "BossHpBar.h"
#include"DiamondAG.h"
#include"ComboHitFont.h"
#include"NotMovingFont.h"
#include"MonsterMovingHP.h"
#include"UINumber.h"
#include"UI_Manager.h"
IMPLEMENT_SINGLETON(CBattle_Manager)


CBattle_Manager::CBattle_Manager()
{
}



HRESULT CBattle_Manager::NativeConstruct()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	m_pMonsterLayer = pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Monster_Battle_Layer"));

	Safe_Release(pGameInstance);

	return S_OK;
}


void CBattle_Manager::Battle_Enter(CEnemy* pEnemy)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CPlayer_Manager* pPlayerManger = CPlayer_Manager::GetInstance();
	Safe_AddRef(pGameInstance);
	Safe_AddRef(pPlayerManger);

	m_iCp = 400;

	//튜토레벨꺼 다 꺼져
	CLayer* TutorialLayer = pGameInstance->Find_Layer(LEVEL_TUTORIAL, TEXT("Layer_TutorialUI"));
	list<CGameObject*>& TutorialList = TutorialLayer->Get_ObjectList();

	for (auto& iter : TutorialList)
	{
		//후후
		dynamic_cast<CUI*>(iter)->RenderChange(false);
	}
	//여기다 플레이어 HP바 만들기
	CPlayer_Manager* m_pPlayer_Manager = CPlayer_Manager::GetInstance();
	CPlayer* pFieldPlayer = pPlayerManger->Get_FieldPlayer();
	CUI_Manager* pUiManager = CUI_Manager::GetInstance();
	CPlayer* player = pUiManager->GetPlayer(m_pPlayer_Manager->Get_MainPlayerIndex());

	//PlayerMovingHP생성
	if (!m_fBattleGo) {
		PlayerMovingHP::MHDESC mhdesc;
		mhdesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		mhdesc.pivotx = 0;
		mhdesc.pivoty = -30;
		mhdesc.UIKind = 0;
		if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_PlayerMovingHP"), &mhdesc))
			return;

		//다이아몬드 생성
		//0 하양이
		DiamondAG::MHDESC diadesc;
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90;
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia0 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		////0 옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90;
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////fulldia[0] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		//1 하양이 
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 1);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia1 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		////1 옥색이 
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 1);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////fulldia[1] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);



		//2 하양이
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 2);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject*  emptydia2 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		////2 옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 2);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////fulldia[2] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		//3 하양이
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 3);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia3 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		////3옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 3);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////	fulldia[3] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);


		//4하양이
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 4);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia4 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);

		////4옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 4);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////	fulldia[4] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);


		//5하양이
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 5);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia5 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);


		////5옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 5);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////	fulldia[5] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);



		//6하양이
		ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		diadesc.pivotx = -90 + (14 * 6);
		diadesc.pivoty = -40;
		diadesc.kind = 0;
		CGameObject* emptydia6 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);


		////6옥색이
		//ZeroMemory(&diadesc, sizeof(DiamondAG::MHDESC));
		//diadesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		//diadesc.pivotx = -90 + (14 * 6);
		//diadesc.pivoty = -40;
		//diadesc.kind = 1;
		////	fulldia[6] = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_DiamondAg"), &diadesc);



		//HP숫자로 뜨는ㄴ것

		//여기서 NotMovingFont 글자
		NotMovingFont::MHDESC combodesc;
		combodesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_Component(TEXT("Com_Transform")));
		combodesc.pivotx = 50;
		combodesc.pivoty = -8;
		combodesc.scaleX = 15;
		combodesc.scaleY = 15;
		combodesc.kind = 1;
		combodesc.UIKind = 1;
		//ComboHitFont 뜨자. 
		CGameObject* combofont =
			pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_NotMovingFont"), &combodesc);
		

		m_fBattleGo = true;


	}
//	CPlayer* pFieldPlayer = pPlayerManger->Get_FieldPlayer();


	_vector vFieldPlayerPos = pFieldPlayer->Get_PlayerPos();

	XMStoreFloat4(&m_vPrePlayerPos, vFieldPlayerPos);

	if (m_pMonsterLayer == nullptr)
		m_pMonsterLayer = pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Monster_Battle_Layer"));


	if (m_pPlayerLayer == nullptr)
		m_pPlayerLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player"));


	if (m_pPlayerBulletLayer == nullptr)
		m_pPlayerBulletLayer = pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet"));

	if (m_pCamera == nullptr)
	{
		m_pCamera = ((CCamera_Default*)pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Camera"), 0));
	}


	_vector vPos[3] = {};
	if ((CEnemy*)pEnemy->Get_IsBoss())
	{
		m_iEnemyNum = 1;
		vPos[0] = XMVectorSet(8.f, 0.f, 8.f, 1.f);
	}

	else
	{
		m_iEnemyNum = 3;
		vPos[0] = XMVectorSet(5.f, 0.f, 8.f, 1.f);
		vPos[1] = XMVectorSet(8.f, 0.f, 8.f, 1.f);
		vPos[2] = XMVectorSet(11.f, 0.f, 8.f, 1.f);
	}

	// Player 위치 이동 여부를 판별하는 변수, 이동시 true가 된다.
	_bool bPlayerMove = false;

	for (_uint i = 0; i < m_iEnemyNum; ++i) {

		// 적 오브젝트 생성 요소 정의
		CEnemy::EnemyDesc Desc;

		// 인자로 위치를 던져줌
		Desc.vPos = vPos[i];
		Desc.eMapType = pEnemy->Get_MapType();

		// 복제
		CEnemy* pEnemyClone = (CEnemy*)pEnemy->Clone(&Desc);

		// 보스라면, 씬을 시작한다.
		if ((CEnemy*)pEnemy->Get_IsBoss())
			pEnemyClone->Set_StartScene();

		m_isBossNameUIRender = true;

		if ((CEnemy*)pEnemy->Get_IsBoss())
		{
	
			//맨티스 글귀
			CUINumber::UINUMDESC bossmonsterhpUI;
			bossmonsterhpUI.tUIInfo.fX = 1310; // g_iWinCX / 2 + 470;
			bossmonsterhpUI.tUIInfo.fY = 140;
			bossmonsterhpUI.tUIInfo.fCX = 15;
			bossmonsterhpUI.tUIInfo.fCY = 15;
			bossmonsterhpUI.kind = 5;
			CGameObject* pBossMonsterHpUI = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_NumberFont"), &bossmonsterhpUI);


			//4개의 UI BossMonster 
			BossHpBar::MONSTERUIBARDESC uidesc;

			uidesc.uiinfo.fX = 700;
			uidesc.uiinfo.fY = g_iWinCX / 2 - 850;
			uidesc.uiinfo.fCX = 205;
			uidesc.uiinfo.fCY = 14;
			uidesc.kind = 2;
			CGameObject* pMonsterUIBar1 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_BossHpBar"), &uidesc);

			ZeroMemory(&uidesc, sizeof(BossHpBar::UIINFO));
			uidesc.uiinfo.fX = 900;
			uidesc.uiinfo.fY = g_iWinCX / 2 - 850;
			uidesc.uiinfo.fCX = 205;
			uidesc.uiinfo.fCY = 14;
			uidesc.kind = 3;
			CGameObject* pMonsterUIBar2 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_BossHpBar"), &uidesc);


			ZeroMemory(&uidesc, sizeof(BossHpBar::UIINFO));
			uidesc.uiinfo.fX = 1100;
			uidesc.uiinfo.fY = g_iWinCX / 2 - 850;
			uidesc.uiinfo.fCX = 205;
			uidesc.uiinfo.fCY = 14;
			uidesc.kind = 1;
			CGameObject* pMonsterUIBar3 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_BossHpBar"), &uidesc);


			ZeroMemory(&uidesc, sizeof(BossHpBar::UIINFO));
			uidesc.uiinfo.fX = 1300;
			uidesc.uiinfo.fY = g_iWinCX / 2 - 850;
			uidesc.uiinfo.fCX = 205;
			uidesc.uiinfo.fCY = 14;
			uidesc.kind = 0;
			CGameObject* pMonsterUIBar4 = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_BossHpBar"), &uidesc);

			////요기서 해도 될런지..
			CPlayer* player = pUiManager->GetPlayer(pPlayerManger->Get_MainPlayerIndex());
			float enemyhp = player->Get_TargetEnemyHP();

			if (enemyhp <= enemyhp / 4)
			{
				if (dynamic_cast<BossHpBar*>(pMonsterUIBar1)->GageOut()) { //다 썼는지 안썼는지 파악해본다.
					dynamic_cast<BossHpBar*>(pMonsterUIBar2)->SetMonsterHPGone(1); //다썼으면 다음 것을 인덱스삼는다.
				}
			}
			else if (enemyhp <= enemyhp)
			{
				if (dynamic_cast<BossHpBar*>(pMonsterUIBar2)->GageOut()) {
					dynamic_cast<BossHpBar*>(pMonsterUIBar3)->SetMonsterHPGone(2);
				}
				if (dynamic_cast<BossHpBar*>(pMonsterUIBar3)->GageOut()) {
					dynamic_cast<BossHpBar*>(pMonsterUIBar4)->SetMonsterHPGone(3);
				}
				else {
					dynamic_cast<BossHpBar*>(pMonsterUIBar1)->SetMonsterHPGone(0);
				}
			}

		
		}
		// 복제된 몬스터의 위치정보를 가져옴
		CTransform* pEnemyTransformCom = dynamic_cast<CTransform*>(pEnemyClone->Get_Component(TEXT("Com_Transform")));

		// 해당 몬스터에 네비를 태운다. 이는 y값을 계산하기 위함.
		CNavigation* pNavigation = nullptr;

		// 몬스터가 가지고 있는 맵 타입의 정보를 가져와 해당 맵의 위치로 이동한다.
		CLayer* pLayer = nullptr;
		CBalseph_Stair* pStair = nullptr;
		switch (pEnemyClone->Get_MapType())
		{
		case Client::MAP_BALSELPH:

			Enemy_MoveToMap(pEnemyClone, _float3(-75.31, 19.25, 0.02), TEXT("Prototype_Component_Navigation_Map_Balseph"));

			if (!bPlayerMove) {
			
				// 플레이어의 위치를 이동한다.
				(*pPlayerManger->Get_VecPlayers())[0]->Set_CurBattlePos(XMVectorSet(-67.72f, 12.55f, -0.036f, 1.f));
				(*pPlayerManger->Get_VecPlayers())[0]->Set_CurBattleRadius(18.f);
				(*pPlayerManger->Get_VecPlayers())[1]->Set_CurBattlePos(XMVectorSet(-67.72f, 12.55f, -0.036f, 1.f));
				(*pPlayerManger->Get_VecPlayers())[1]->Set_CurBattleRadius(18.f);


				Player_MoveToMap((*pPlayerManger->Get_VecPlayers())[0], _float3(-67.72f, 12.55f, -0.036f), TEXT("Prototype_Component_Navigation_Map_Balseph"));
				Player_MoveToMap((*pPlayerManger->Get_VecPlayers())[1], _float3(-70.72f, 12.55f, -0.036f), TEXT("Prototype_Component_Navigation_Map_Balseph"));
				bPlayerMove = true;
			}
			

			pLayer = pGameInstance->Find_Layer(LEVEL_LORD_BALSEPH, LAYER_MAPTOOL_Balseph_Stair);

			if (nullptr != pLayer) {
				pStair = dynamic_cast<CBalseph_Stair*>(*pLayer->Get_ObjectList().begin());
				pStair->Set_EnemyTarget(dynamic_cast<CLord_Balseph*>(pEnemyClone));
			}

			m_pLord_Balseph = dynamic_cast<CLord_Balseph*>(pEnemyClone);

			break;

		case Client::MAP_FIREAVATAR:
			Enemy_MoveToMap(pEnemyClone, _float3(-91.22, 2.42, 0.32), TEXT("Prototype_Component_Navigation_Map_FireAvatar"));

			if (!bPlayerMove) {
				// 플레이어의 위치를 이동한다.
				Player_MoveToMap(pFieldPlayer, _float3(-91.22, 2.42, 0.32), TEXT("Prototype_Component_Navigation_Map_FireAvatar"));
				bPlayerMove = true;
			}

			break;

		case Client::MAP_BATTLE02:
			Enemy_MoveToMap(pEnemyClone, _float3(995.f + 5.f*i, 0.f, 8.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"));
			pEnemyTransformCom->TurnAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f));
			pEnemyClone->Set_CurBattlePos(XMVectorSet(1005.7f, 0.f, 2.31f, 1.f));
			pEnemyClone->Set_CurBattleRadius(18.f);
			if (!bPlayerMove) {
				for (auto pPlayer : *pPlayerManger->Get_VecPlayers())
				{
					pPlayer->Set_CurBattlePos(XMVectorSet(1005.7f, 0.f, 2.31f, 1.f));
					pPlayer->Set_CurBattleRadius(18.f);

					Player_MoveToMap(pPlayer, _float3(1000.f, 0.f, 0.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"));
				}
				bPlayerMove = true;
			}

			break;

		case Client::MAP_BATTLE04:
			Enemy_MoveToMap(pEnemyClone, _float3(2005.f, 0.f, 0.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_04"));

			if (!bPlayerMove) {
				// 플레이어의 위치를 이동한다.
				for (auto pPlayer : *pPlayerManger->Get_VecPlayers())
				{
					Player_MoveToMap(pPlayer, _float3(2005.f, 0.f, 0.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_04"));
				}
				bPlayerMove = true;
			}

			break;

		default:
			MSG_BOX(TEXT("Wrong Enemy MapData Detected! Set Battle or Dungeon Map_Type"));
			break;
		}
		

		pEnemyClone->Set_Battle(true);

		m_pMonsterLayer->Add_GameObject(pEnemyClone);

		m_vecMonsters.push_back(pEnemyClone);
	}

	//vPos[0] = XMVectorSet(5.f, 0.f, 2.f, 1.f);
	//vPos[1] = XMVectorSet(8.f, 0.f, 2.f, 1.f);
	//vPos[2] = XMVectorSet(11.f, 0.f, 2.f, 1.f);

	_uint iCnt = 0;

	vector<CEnemy*>::iterator iter = m_vecMonsters.begin();
	for (auto pPlayer : *pPlayerManger->Get_VecPlayers())
	{

		pPlayer->Set_VecMonstersIter(iter);
		pPlayer->Set_TargetEnemy(*iter);
		pPlayer->Set_Battle(true);
		pPlayer->Set_EquipmentBattle(true);
		//pPlayer->Set_PlayerPos(vPos[iCnt++]); -> 위에서 바꿀것, 아니면 switch 문을 사용해 이동할 맵의 정보에 따라 다른 값을 여기서 넣는것도 가능
		pPlayer->Set_VecMonsters(&m_vecMonsters);
		++iter;
		if (iter == m_vecMonsters.end())
		{
			iter = m_vecMonsters.begin();
		}
	}

	m_pCamera->Set_PreTargetPos(pPlayerManger->Get_MainPlayer()->Get_TargetPos());

	m_bBattle = true;
	m_pVecPlayers = pPlayerManger->Get_VecPlayers();
	m_pCamera->Set_CameraState(CCamera_Default::CAMERA_STATE_BATTLE_ENTER);

	MonsterMovingHP::MHDESC mhdesc;
	//mhdesc.targetTransform = (CTransform*)(*iter)->Get_Component(TEXT("Com_Transform"));
	mhdesc.pivotx = 10;
	mhdesc.pivoty = -90;
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, L"Layer_BattleUI", TEXT("Prototype_GameObject_MonsterMovingHP"), &mhdesc))
		return;


	//여기서 NotMovingFont 글자
	NotMovingFont::MHDESC mondesc;
	mondesc.targetTransform = (CTransform*)(m_pPlayer_Manager->Get_MainPlayer()->Get_TargetEnemy()->Get_Component(TEXT("Com_Transform")));
	mondesc.pivotx = 50;
	mondesc.pivoty = -18;
	mondesc.scaleX = 15;
	mondesc.scaleY = 15;
	mondesc.kind = 1;
	mondesc.UIKind = 2;
	//ComboHitFont 뜨자. 
	CGameObject* combofont =
		pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_NotMovingFont"), &mondesc);



	//numberfont도 넣어주자. 

	//CUINumber::UINUMDESC monsterdesc;
	//monsterdesc.tUIInfo.fX = 1310; // g_iWinCX / 2 + 470;
	//monsterdesc.tUIInfo.fY = 140;
	//monsterdesc.tUIInfo.fCX = 15;
	//monsterdesc.tUIInfo.fCY = 15;
	//monsterdesc.kind = 7;
	//CGameObject* pBossMonsterHpUI = pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_NumberFont"), &bossmonsterhpUI);



	Safe_Release(pGameInstance);
	Safe_Release(pPlayerManger);
}

void CBattle_Manager::Battle_End()
{
	CPlayer_Manager* pPlayerManger = CPlayer_Manager::GetInstance();
	Safe_AddRef(pPlayerManger);
	CPlayer* pFieldPlayer = pPlayerManger->Get_FieldPlayer();

	_uint iCnt = 0;
	for (auto pPlayer : *pPlayerManger->Get_VecPlayers())
	{
		pPlayer->Set_Battle(false);
		pPlayer->Set_EquipmentBattle(false);


	}


	CNavigation* pNavigation = nullptr;
	CTransform* pAlphen_TransformCom = nullptr;


	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	pNavigation = pFieldPlayer->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_Tutorial"));

	pAlphen_TransformCom = dynamic_cast<CTransform*>(pFieldPlayer->Get_Component(TEXT("Com_Transform")));
	pFieldPlayer->Set_PlayerPos(XMLoadFloat4(&m_vPrePlayerPos));
	pNavigation->Find_My_Cell(XMLoadFloat4(&m_vPrePlayerPos));
	pAlphen_TransformCom->Move(m_vPrePlayerPos.x, pAlphen_TransformCom->Get_Height(pNavigation), m_vPrePlayerPos.z);

	pPlayerManger->Set_MainPlayer(0);


	Safe_Release(pPlayerManger);
	m_bBattle = false;
}

void CBattle_Manager::Tick(_double TimeDelta)
{
	if (m_bBattle)
	{
		CPlayer_Manager* pPlayerManger = CPlayer_Manager::GetInstance();
		Safe_AddRef(pPlayerManger);




		//몸통 부딪힘
		for (auto Player : *m_pVecPlayers)
		{
			if (Player->Get_Overlap())
				continue;
			CCollider* pPlayerCollider = (CCollider*)Player->Get_Component(TEXT("Com_SPHERE"));
			for (auto Enmey : m_vecMonsters)
			{

				CCollider* pMonsterCollider = (CCollider*)Enmey->Get_Component(TEXT("Com_SPHERE"));

				_float fDistance = 0.f;
				_vector vDir;
				if (pMonsterCollider->Collsion_Sphere(pPlayerCollider, &fDistance, &vDir))
				{
					Player->Set_Move_In_Circle(XMVectorSetY(vDir, 0.f)*fDistance);

				}

			}
		}




		//몸통 몬스터끼리 부딪힘
		for (_uint j = 0; j < m_vecMonsters.size(); ++j)
		{
			CCollider* pMonsterCollider1 = (CCollider*)m_vecMonsters[j]->Get_Component(TEXT("Com_SPHERE"));


			for (_uint i = j + 1; i < m_vecMonsters.size(); ++i)
			{

				CCollider* pMonsterCollider2 = (CCollider*)m_vecMonsters[i]->Get_Component(TEXT("Com_SPHERE"));

				if (pMonsterCollider1->Collsion_Sphere(pMonsterCollider2))
				{


					_float fDistance = 0.f;
					_vector vDir;
					if (pMonsterCollider2->Collsion_Sphere(pMonsterCollider1, &fDistance, &vDir))
					{
						m_vecMonsters[j]->Set_Move_In_Circle(XMVectorSetY(vDir, 0.f)*fDistance);

					}

				}

			}
		}





		CurrentCp--;



		//플레이어 공격  중복처리어떻게하는게 좋을까. 콜라이더 속성을 바꾸면 안되고 나자신이 알아야한다.

		for (_uint i = 0; i < (*m_pVecPlayers).size(); ++i)
		{
			CCollider* pPlayerCollider = (CCollider*)(*m_pVecPlayers)[i]->Get_Component(TEXT("Com_AttackSPHERE"));
			if (!pPlayerCollider->Get_On())
			{
				for (auto Enemy : m_vecMonsters)
				{
					switch ((PLAYER_TYPE)i)
					{
					case Client::CBattle_Manager::PLAYER_ALPHEN:
						Enemy->Set_NewCollisionAlphen(false);
						break;
					case Client::CBattle_Manager::PLAYER_SHIONNE:
						Enemy->Set_NewCollisionShionne(false);
						break;
					case Client::CBattle_Manager::PLAYER_KISARA:
						Enemy->Set_NewCollisionKisara(false);
						break;
					case Client::CBattle_Manager::PLAYER_RINWELL:
						Enemy->Set_NewCollisionRinwell(false);
						break;
					}
				}
				continue;
			}

			for (auto Enemy : m_vecMonsters)
			{
				if (Enemy->Get_Invincible())
				{
					continue;
				}

				CCollider* pMonsterCollider = (CCollider*)Enemy->Get_Component(TEXT("Com_SPHERE"));
				CGameInstance* pGameInstance = CGameInstance::GetInstance();

				if (pMonsterCollider->Collsion_Sphere(pPlayerCollider))
				{
					//Enemy->Set_Collision(true);


					_uint hit = Enemy->Get_Hits();
					DAMAGEINFO temp = (*m_pVecPlayers)[i]->Get_AttackDamageInfo();



					_vector      vForward = Enemy->Get_EnemyLook();
					_vector		 vPosition = Enemy->Get_EnemyPos();
					_vector      vTarget = XMVector3Normalize((*m_pVecPlayers)[i]->Get_PlayerPos() - vPosition);

					_float fDot = XMVectorGetX(XMVector3Dot(vForward, vTarget));

					if (fDot >= 0) //적이 앞에
					{
						temp.e_DamageDir = DAMAGE_DIR_FRONT;
					}
					else {		   //적이 뒤에
						temp.e_DamageDir = DAMAGE_DIR_BACK;
					}

					temp.e_PlayerType = (PLAYER_TYPE)i;

					_vector vPos = (*m_pVecPlayers)[i]->Get_PlayerPos();
					_vector vDir = XMVector3Normalize(XMVectorSetY(Enemy->Get_EnemyPos(), 0.f) - XMVectorSetY(vPos, 0.f));
					Enemy->Set_KnockBackDir(vDir);


					


					switch ((PLAYER_TYPE)i)
					{
					case Client::CBattle_Manager::PLAYER_ALPHEN:
						if (!Enemy->Get_NewCollisionAlphen())
						{
							Enemy->Set_NewCollisionAlphen(true);
							Enemy->Set_HitDamageInfo(temp);
							Enemy->Set_Damage(1); 
							

							//여기서 처마즘.
							ComboHitFont::MHDESC combodesc;

							combodesc.targetTransform = (CTransform*)Enemy->Get_Component(TEXT("Com_Transform"));
							combodesc.pivotx = uidX(dre); //랜덤으로 바꾸어야함.
							combodesc.pivoty = uidY(dre);
							combodesc.scaleX = 40;
							combodesc.scaleY = 40;
							combodesc.kind = 0;
							combodesc.UIKind = 0;
							combodesc.HitDamage = 1;  //여기 나중에 진구찡이 tmp로 넣어준다고 했슴.

													  //ComboHitFont 뜨자. 
							CGameObject* combofont =
								pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_ComboHitFont"), &combodesc);



							Set_CameraShake(temp.e_DamageType);
							Enemy->Set_Collision(true);
							Enemy->Set_Hits();
						}
						break;

					case Client::CBattle_Manager::PLAYER_SHIONNE:
						if (!Enemy->Get_NewCollisionShionne())
						{
							Enemy->Set_NewCollisionShionne(true);
							Enemy->Set_HitDamageInfo(temp);
							Enemy->Set_Damage(1);

							//여기서 처마즘.
							ComboHitFont::MHDESC combodesc;

							combodesc.targetTransform = (CTransform*)Enemy->Get_Component(TEXT("Com_Transform"));
							combodesc.pivotx = uidX(dre); //랜덤으로 바꾸어야함.
							combodesc.pivoty = uidY(dre);
							combodesc.scaleX = 40;
							combodesc.scaleY = 40;
							combodesc.kind = 0;
							combodesc.UIKind = 0;
							combodesc.HitDamage = 1;  //여기 나중에 진구찡이 tmp로 넣어준다고 했슴.

													  //ComboHitFont 뜨자. 
							CGameObject* combofont =
								pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_BattleUI", TEXT("Prototype_GameObject_ComboHitFont"), &combodesc);



							Set_CameraShake(temp.e_DamageType);
							Enemy->Set_Collision(true);
							Enemy->Set_Hits();
						};
						break;
					case Client::CBattle_Manager::PLAYER_KISARA:
						if (!Enemy->Get_NewCollisionKisara())
						{
							Enemy->Set_NewCollisionKisara(true);
							Enemy->Set_HitDamageInfo(temp);
							Enemy->Set_Damage(1);
							Set_CameraShake(temp.e_DamageType);
							Enemy->Set_Collision(true);
							Enemy->Set_Hits();
						}
						break;
					case Client::CBattle_Manager::PLAYER_RINWELL:
						if (!Enemy->Get_NewCollisionRinwell())
						{
							Enemy->Set_NewCollisionRinwell(true);
							Enemy->Set_HitDamageInfo(temp);
							Enemy->Set_Damage(1);
							Set_CameraShake(temp.e_DamageType);
							Enemy->Set_Collision(true);
							Enemy->Set_Hits();
						}
						break;
					}



				}

			}
		}





		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		//몬스터가 픙레이어 공격
		for (_uint j = 0; j < m_vecMonsters.size(); ++j)
		{
			CCollider* pMonsterCollider = (CCollider*)m_vecMonsters[j]->Get_Component(TEXT("Com_AttackSPHERE"));
			if (!pMonsterCollider->Get_On())
			{
				for (auto Player : (*m_pVecPlayers))
				{
					Player->Set_IsHitedMonster(j, false);
				}
				continue;
			}

			for (_uint i = 0; i < (*m_pVecPlayers).size(); ++i)
			{
				if ((*m_pVecPlayers)[i]->Get_Invincible())
				{
					continue;
				}

				CCollider* pPlayerCollider = (CCollider*)(*m_pVecPlayers)[i]->Get_Component(TEXT("Com_SPHERE"));

				if (pPlayerCollider->Collsion_Sphere(pMonsterCollider))
				{

					if ((*m_pVecPlayers)[i]->Get_IsHitedMonster(j) == false)
					{
						(*m_pVecPlayers)[i]->Set_IsHitedMonster(j, true);
						DAMAGEINFO temp = m_vecMonsters[j]->Get_AttackDamageInfo();
						temp.e_DamageDir = DAMAGE_DIR_FRONT;

						_vector      vForward = (*m_pVecPlayers)[i]->Get_PlayerPos();
						_vector		 vPosition = (*m_pVecPlayers)[i]->Get_PlayerLook();
						_vector      vTarget = XMVector3Normalize((m_vecMonsters[j]->Get_EnemyPos() - vPosition));

						_float fDot = XMVectorGetX(XMVector3Dot(vForward, vTarget));

						if (fDot >= 0) //적이 앞에
						{
							temp.e_DamageDir = DAMAGE_DIR_FRONT;
						}
						else {		   //적이 뒤에
							temp.e_DamageDir = DAMAGE_DIR_BACK;
						}

						temp.e_DamageType = DAMAGE_TYPE_SMALL;


						//콤보히트 바뀐거 정정..
						//히트정보 아직 안넘겨줘서 금요일에 합치고 넘겨줘야함 


						//_vector vPos = (*m_pVecPlayers)[i]->Get_PlayerPos();
						//_vector vDir = XMVector3Normalize(XMVectorSetY(XMVectorSetY(vPos, 0.f) - m_vecMonsters[j]->Get_EnemyPos(), 0.f));
						//(*m_pVecPlayers)[i]->Set_KnockBackDir(vDir);
						//(*m_pVecPlayers)[i]->Set_Collision(true);
						//(*m_pVecPlayers)[i]->Set_HitDamageInfo(temp);

				
					
					}

				}

			}
		}

		//플레이어 마법 혹은 불릿이 공격

		if (m_pPlayerBulletLayer->Get_ObjectList().size() > 0)
		{

			for (auto Bullet : m_pPlayerBulletLayer->Get_ObjectList())
			{
				CCollider* PlayerBulletCollider = (CCollider*)Bullet->Get_Component(TEXT("Com_AttackSPHERE"));
				if (PlayerBulletCollider == nullptr)
				{
					continue;
				}

				for (auto Monster : m_vecMonsters)
				{
					CCollider* MonsterCollider = (CCollider*)Monster->Get_Component(TEXT("Com_SPHERE"));

					if (MonsterCollider->Collsion_Sphere(PlayerBulletCollider))
					{
						Bullet->Set_Dead(true);
						Monster->Set_Damage(1);

					}
				}
			}
		}

		vector<CEnemy*>::iterator Target_Iter = pPlayerManger->Get_MainPlayer()->Get_Targeting();

		if (m_vecMonsters.size() > 0)
		{
			auto iter = m_vecMonsters.begin();
			for (iter; iter != m_vecMonsters.end(); )
			{
				if ((*iter)->Get_Hp() <= 0)
				{



					for (auto Player : *m_pVecPlayers)
					{
						vector<CEnemy*>::iterator Player_Iter = Player->Get_Targeting();
						if (Player_Iter == iter)
						{
							//여기서 같은애들 기록한다면 그떄 넘긴다. 나머지는 ㄹ
							//현재플레이어의 타게팅과 죽을려믄 몬스터의 타게팅이 같다면 
							Player_Iter++;
							if (Player_Iter == m_vecMonsters.end())
							{
								Player_Iter = m_vecMonsters.begin();
							}
							//layer->Set_VecMonstersIter(Player_Iter);
							Player->Set_TargetEnemy(*Player_Iter);
						}
						else {
							Player->Set_TargetEnemy(*Player_Iter);
						}

					}

					iter = m_vecMonsters.erase(iter);


					//여기서 지운다면 iter의 순서가 바뀔수있다ㅣ.

					//타게팅을 바꿔야할수도 .

					//중간에있는애가 죽는다면 

				}
				else
					++iter;
			}

		}



		if (m_vecMonsters.size() > 0)
		{
			auto iter = m_vecMonsters.begin();
			for (iter; iter != m_vecMonsters.end(); )
			{
				for (auto Player : *m_pVecPlayers)
				{
					CEnemy* pEnemy = Player->Get_TargetEnemy();
					if (pEnemy == *iter)
					{

						Player->Set_VecMonstersIter(iter);
					}
				}
				++iter;

			}
		}

		if (m_vecMonsters.size() <= 0)
		{
			m_pCamera->Set_CameraState(CCamera_Default::CAMERA_STATE_BATTLE_END);
		}



		if (m_pMonsterLayer->Get_ObjectList().size() <= 0)
		{

			m_bBattle = false;
			Battle_End();
		}





		Safe_Release(pPlayerManger);

	}









}

void CBattle_Manager::Set_CameraShake(DAMAGE_TYPE eDamageType)
{

	switch (eDamageType)
	{
	case Client::DAMAGE_TYPE_DOWN:
		m_pCamera->Set_CommonShake(true);
		break;
	case Client::DAMAGE_TYPE_LARGE:
		m_pCamera->Set_CommonShake(true);
		break;
	case Client::DAMAGE_TYPE_SMALL:
		m_pCamera->Set_CommonShake(true);
		break;

	}
}

HRESULT CBattle_Manager::Ready_Map(const char * pModelFilePath, const char * pModelFileName)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	char szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	TCHAR szCurPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFullPath, (int)strlen(szFullPath), szCurPath, MAX_PATH);

	// 파일을 연다.
	HANDLE		hFile = CreateFile(szCurPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD		dwByte = 0;
	DWORD		dwStrByte = 0;

	for (auto& Obj : m_vMapObject)
		Safe_Release(Obj);
	m_vMapObject.clear();

	CMapObject::MAPOBJDesc Desc;

	while (true)
	{
		// 문자열 메모리의 크기를 읽음
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);

		// 읽어온 메모리의 크기만큼 문자열을 할당하고, 해당 문자열을 읽어옴
		TCHAR*	pName = new TCHAR[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);

		// 잘못됬을 경우 해당 메모리 삭제 및 멈추기
		if (0 == dwByte)
		{
			delete[] pName;
			break;
		}

		// Desc에 정보 넣기
		Desc.pPrototype_ObjectName = pName;

		// Collider 정보 설정
		Desc.fColliderSize = _float3(10.f, 10.f, 10.f);

		// 행렬 정보를 설정
		ReadFile(hFile, &Desc.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

		// 맵 여부 확인
		ReadFile(hFile, &Desc.m_bMap, sizeof(_bool), &dwByte, nullptr);

		Desc.eLevel = LEVEL_STATIC;

		// 해당 오브젝트의 이름이 WarpGate 일 경우
		if (!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_WarpGate"))) {
			//static _uint iWayPointNumber = 0;

			//// WayPoint Desc값을 채운다.
			//CWayPoint::WayPoint_DESC WayPointDesc;
			//WayPointDesc.iPointNumber = iWayPointNumber++;

			//// 위치 설정
			//memcpy(&WayPointDesc.fPos, &Desc.TransformMatrix.m[CTransform::STATE_POSITION][0], sizeof(_float3));

			//// 방향 설정
			//memcpy(&WayPointDesc.fDir, &Desc.TransformMatrix.m[CTransform::STATE_LOOK][0], sizeof(_float3));

			//CWayPoint* pWayPoint = dynamic_cast<CWayPoint*>(pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, LAYER_MAPTOOL_WayPoints, TEXT("Prototype_GameObject_WayPoint"), &WayPointDesc));
			//m_vWayPoints.push_back(pWayPoint);
		}
		else {
			// 맵 오브젝트 생성
			CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, LAYER_MAP_BATTLE, TEXT("Prototype_GameObject_MapObject"), &Desc));
			m_vMapObject.push_back(pMapObject);
		}
	}

	// 파일을 닫는다.
	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBattle_Manager::Enemy_MoveToMap(CEnemy * pEnemy, _float3 fPos, const TCHAR * pTagNaviCompoent)
{
	CNavigation* pNavigation = pEnemy->SetUp_Navigation(pTagNaviCompoent);

	if (nullptr == pNavigation) {
		MSG_BOX(TEXT("CBattle_Manager::Enemy_MoveToMap : Failed to Create NaviCom"));
		return E_FAIL;
	}

	CTransform* pEnemyTransformCom = dynamic_cast<CTransform*>(pEnemy->Get_Component(TEXT("Com_Transform")));

	// Enemy의 위치를 특정 좌표로 이동
	pEnemyTransformCom->Move(fPos);

	// 현재 위치한 네비 셀을 설정
	pNavigation->Find_My_Cell(XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f));

	// 이동된 좌표에서 네비메쉬에 따라 y값을 계산하여 해당 높이로 이동
	pEnemyTransformCom->Move(fPos.x, pEnemyTransformCom->Get_Height(pNavigation), fPos.z);

	_float fHeight = pEnemyTransformCom->Get_Height(pNavigation);
	pEnemy->Set_CurBattleMap_Height(fHeight);

	// y값을 계산하였으니 필요없어진 NaviCom을 지운다.
	return pEnemy->Delete_NaviCom();
}

HRESULT CBattle_Manager::Player_MoveToMap(CPlayer * pPlayer, _float3 fPos, const TCHAR * pTagNaviCompoent)
{
	CNavigation* pNavigation = pPlayer->SetUp_Navigation(pTagNaviCompoent);

	if (nullptr == pNavigation) {
		MSG_BOX(TEXT("CBattle_Manager::Enemy_MoveToMap : Failed to Create NaviCom"));
		return E_FAIL;
	}

	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	// Enemy의 위치를 특정 좌표로 이동
	pPlayerTransformCom->Move(fPos);

	// 현재 위치한 네비 셀을 설정
	pNavigation->Find_My_Cell(XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f));

	_float fHeight = pPlayerTransformCom->Get_Height(pNavigation);
	pPlayer->Set_CurBattleMap_Height(fHeight);

	// 이동된 좌표에서 네비메쉬에 따라 y값을 계산하여 해당 높이로 이동
	pPlayerTransformCom->Move(fPos.x, fHeight, fPos.z);

	pPlayerTransformCom->TurnAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	// y값을 계산하였으니 필요없어진 NaviCom을 지운다.
	return pPlayer->Delete_NaviCom();
}

void CBattle_Manager::Free()
{

}