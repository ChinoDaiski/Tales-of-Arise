#include "stdafx.h"
#include "..\Public\Level_Balseph.h"
#include "GameInstance.h"

#include "MapObject.h"
#include "WayPoint.h"
#include "WayPoint_Pos.h"
#include "Player.h"
#include "Navigation.h"
#include "Level_Loading.h"
#include "Owl.h"
#include "UI_Owl.h"
#include "Balseph_Stair.h"
#include "Lord_Balseph.h"

CLevel_Balseph::CLevel_Balseph(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
	, m_pBattle_Manager(CBattle_Manager::GetInstance())
{
	Safe_AddRef(m_pBattle_Manager);
}

HRESULT CLevel_Balseph::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	//if (FAILED(Ready_Lights()))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Balseph(TEXT("Layer_Balseph"))))
		return E_FAIL;

	if (FAILED(Ready_Map("../Bin/Resources/Dat/Stage03/", "MeshList.dat", XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(Ready_WatPoint_Pos(XMVectorSet(-53.54, 12.55, -0.1, 1.f), XMVectorSet(-56.75, 12.55, 0.23, 1.f))))
		return E_FAIL;
}

void CLevel_Balseph::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pBattle_Manager->Tick(TimeDelta);

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Balseph::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("튜토리얼 레벨입니다."));

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (m_isBattle) {

		wsprintf(HitMsg, TEXT("HITS"));
		if (FAILED(pGameInstance->Render_Fonts(TEXT("Font_Javan40"), HitMsg, _float2(1787, 217/*g_iWinCX / 2 + 700, g_iWinCY / 2 - 200*/), XMVectorSet(0.9f, 1.f, 0.7f, 1.f))))
			return E_FAIL;

		wsprintf(DamageMsg, TEXT("DAMAGES"));
		if (FAILED(pGameInstance->Render_Fonts(TEXT("Font_Javan40"), DamageMsg, _float2(1747, 267/*g_iWinCX / 2 + 650, g_iWinCY / 2 - 150*/), XMVectorSet(0.9f, 1.f, 0.7f, 1.f))))
			return E_FAIL;

		wsprintf(ComboHit, TEXT("%d"), combohitNum);
		if (FAILED(pGameInstance->Render_Fonts(TEXT("Font_Javan50"), ComboHit, _float2(1680, 150), XMVectorSet(0.9f, 1.f, 0.7f, 1.f))))
			return E_FAIL;


		wsprintf(DamageHit, TEXT("%d"), damageNum);
		if (FAILED(pGameInstance->Render_Fonts(TEXT("Font_Javan50"), DamageHit, _float2(1680, 210), XMVectorSet(0.9f, 1.f, 0.7f, 1.f))))
			return E_FAIL;

	}

	return S_OK;

}

HRESULT CLevel_Balseph::Ready_Layer_Balseph(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pLord_Balseph = dynamic_cast<CLord_Balseph*>(pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, pLayerTag, TEXT("Prototype_GameObject_Balseph")));

	if (nullptr == m_pLord_Balseph)
		return E_FAIL;

	dynamic_cast<CTransform*>(m_pLord_Balseph->Get_Component(TEXT("Com_Transform")))->Move(_float3(-84.9f, 12.55f, 0.09f));

	CNavigation* pNavigation = nullptr;
	CTransform* pBalseph_TransformCom = nullptr;


	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	pNavigation = m_pLord_Balseph->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_Balseph"));

	pBalseph_TransformCom = dynamic_cast<CTransform*>(m_pLord_Balseph->Get_Component(TEXT("Com_Transform")));
	pBalseph_TransformCom->Move(-84.9f, 12.55f, 0.09f);
	pNavigation->Find_My_Cell(XMVectorSet(-84.9f, 12.55f, 0.09f, 1.f));
	pBalseph_TransformCom->Move(-84.9f, pBalseph_TransformCom->Get_Height(pNavigation), 0.09f);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Skybox 생성
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, LAYER_MAPTOOL_Skybox, TEXT("Prototype_GameObject_Sky")))
		return E_FAIL;

	// Alphen 생성
	CLayer* pLayerPlayer = pGameInstance->Find_Layer(LEVEL_STATIC, pLayerTag);

	if (nullptr == pLayerPlayer) {
		MSG_BOX(TEXT("CLevel_Balseph : No Player Detected"));
		return E_FAIL;
	}

	m_pPlayerAlphen = dynamic_cast<CPlayer*>(*pLayerPlayer->Get_ObjectList().begin());

	if (nullptr == m_pPlayerAlphen)
		return E_FAIL;

	CNavigation* pNavigation = nullptr;
	CTransform* pAlphen_TransformCom = nullptr;

	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_Balseph"));

	pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	pAlphen_TransformCom->Move(43.37, -2.45, 0.02);
	pNavigation->Find_My_Cell(XMVectorSet(43.37, -2.45, 0.02, 1.f));
	pAlphen_TransformCom->Move(43.37, pAlphen_TransformCom->Get_Height(pNavigation), 0.02);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_Map(const char * pModelFilePath, const char * pModelFileName, _vector vCenterPos)
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
			delete[]pName;
			break;
		}

		// Desc에 정보 넣기
		Desc.pPrototype_ObjectName = pName;

		// Collider 정보 설정
		Desc.fColliderSize = _float3(10.f, 10.f, 10.f);

		// 행렬 정보를 설정
		ReadFile(hFile, &Desc.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

		_float4x4 matWorld4x4;
		XMStoreFloat4x4(&matWorld4x4, XMMatrixIdentity());

		_float3 fPos;
		XMStoreFloat3(&fPos, vCenterPos);
		memcpy(&matWorld4x4.m[CTransform::STATE_POSITION], &fPos, sizeof(_float3));

		// 파일에서 읽은 값과 인자로 받은 월드 위치를 곱하여 오브젝트들을 해당 위치로 이동시킨다.
		XMStoreFloat4x4(&Desc.TransformMatrix, XMMatrixMultiply(XMLoadFloat4x4(&Desc.TransformMatrix), XMLoadFloat4x4(&matWorld4x4)));



		// 맵 여부 확인
		ReadFile(hFile, &Desc.m_bMap, sizeof(_bool), &dwByte, nullptr);

		Desc.eLevel = LEVEL_STATIC;

		if (!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02_NaviMap")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04_NaviMap"))
			) {
			Desc.eLevel = LEVEL_STATIC;
		}

		// 해당 오브젝트의 이름이 Owl일 경우
		if (!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Owl_001"))) {
			// 맵 오브젝트 생성
			COwl::OwlDesc OwlDesc;
			OwlDesc.TransformMatrix = Desc.TransformMatrix;
			OwlDesc.fDistance = 5.f;
			OwlDesc.pModelTag = Desc.pPrototype_ObjectName;
			OwlDesc.pTargetPlayer = m_pPlayerAlphen;

			COwl* pOwl = dynamic_cast<COwl*>(pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, LAYER_MAPTOOL_Owls, TEXT("Prototype_GameObject_Owl"), &OwlDesc));
			// 여기서 UI 매니저에 Owl의 정보와 UI를 생성하여 넣어준다.
			// ~~
		}
		// 해당 오브젝트의 이름이 Prototype_Component_Model_Stair_Object 일 경우
		else if (!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Stair_Object"))) {
			CBalseph_Stair::Balseph_StairDesc StairDesc;
			StairDesc.pModelTag = TEXT("Prototype_Component_Model_Stair_Object");
			StairDesc.pTargetEnemy = m_pLord_Balseph;
			StairDesc.TransformMatrix = Desc.TransformMatrix;
			StairDesc.pTargetPlayer = m_pPlayerAlphen;

			CBalseph_Stair* pStairObject = dynamic_cast<CBalseph_Stair*>(pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, LAYER_MAPTOOL_Balseph_Stair, TEXT("Prototype_GameObject_Balseph_Stair"), &StairDesc));
		}
		else {
			// 맵 오브젝트 생성
			CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, LAYER_MAPTOOL_PlacedObjects, TEXT("Prototype_GameObject_MapObject"), &Desc));
			m_vMapObject.push_back(pMapObject);
		}
	}

	// 파일을 닫는다.
	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_WatPoint(LEVEL eNextLevel, _vector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CWayPoint::WayPoint_DESC WayDesc;
	WayDesc.eLevel = LEVEL_LORD_BALSEPH;		// 이동할다음 레벨
	XMStoreFloat3(&WayDesc.fPos, vPos);			// 오브젝트 생성위치
	WayDesc.pTargetPlayer = m_pPlayerAlphen;	// 타겟이 되는 플레이어 오브젝트 정보

	CWayPoint* pWayPoint = dynamic_cast<CWayPoint*>(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, LAYER_WayPoints, TEXT("Prototype_GameObject_WayPoint"), &WayDesc));
	pWayPoint->Set_TargetPlayer(m_pPlayerAlphen);

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pWayPoint)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_WatPoint_Pos(_vector vPos, _vector vMovePos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CWayPoint_Pos::WayPoint_DESC WayDesc;
	XMStoreFloat3(&WayDesc.fPos, vPos);				// 오브젝트 생성위치
	XMStoreFloat3(&WayDesc.fMovePos, vMovePos);		// 오브젝트 생성위치
	WayDesc.pTargetPlayer = m_pPlayerAlphen;		// 타겟이 되는 플레이어 오브젝트 정보

	CWayPoint_Pos* pWayPoint = dynamic_cast<CWayPoint_Pos*>(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, LAYER_WayPoints, TEXT("Prototype_GameObject_WayPoint_Pos"), &WayDesc));
	pWayPoint->Set_TargetPlayer(m_pPlayerAlphen);

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pWayPoint)
		return E_FAIL;

	return S_OK;
}

CLevel_Balseph * CLevel_Balseph::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CLevel_Balseph*	pInstance = new CLevel_Balseph(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CLevel_Tutorial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Balseph::Free()
{
	__super::Free();

	Safe_Release(m_pBattle_Manager);
}