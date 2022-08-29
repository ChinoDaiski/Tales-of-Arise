#include "stdafx.h"
#include "..\Public\Level_Test.h"
#include "GameInstance.h"
#include "Camera_Default.h"
#include"Player.h"
#include "MapObject.h"
CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CLevel(pDevice, pDeviceContext)
	, m_pBattle_Manager(CBattle_Manager::GetInstance())
{
	Safe_AddRef(m_pBattle_Manager);
}

HRESULT CLevel_Test::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : NativeConstruct : NativeConstruct", E_FAIL);

	if (FAILED(Ready_Lights()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : NativeConstruct : Ready_Lights", E_FAIL);


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;



	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Boar(TEXT("Layer_Boar"))))
		return E_FAIL;

	if (FAILED(Ready_Map("../Bin/Resources/Dat/Stage08/", "MeshList.dat", XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(Ready_Map_Battle02(XMVectorSet(1000.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_pBattle_Manager->Tick(TimeDelta);



}

HRESULT CLevel_Test::Render()
{
	if (FAILED(__super::Render()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Render : Render", E_FAIL);

	SetWindowText(g_hWnd, TEXT("Test"));

	return S_OK;
}


HRESULT CLevel_Test::Ready_Map_Battle02(_vector vCenterPos)
{

	_float3 fPos;
	XMStoreFloat3(&fPos, vCenterPos);


	return Ready_Map("../Bin/Resources/Dat/Stage08/", "MeshList.dat", vCenterPos);
}

HRESULT CLevel_Test::Ready_Layer_Boar(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CEnemy::EnemyDesc Desc;

	Desc.vPos = XMVectorSet(0.f, 0.f, 5.f, 1.f);
	Desc.eMapType = MAP_BATTLE02;

	CEnemy* pEnemy = dynamic_cast<CEnemy*>(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Boar"), &Desc));

	CBattle_Manager* pBattleManager = CBattle_Manager::GetInstance();


	pBattleManager->Enemy_MoveToMap(pEnemy, _float3(995.f + 5.f, 0.f, 8.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Ready_Lights : Add_Lights", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.eType = CCamera::CAMERA;
	CameraDesc.vEye = _float3(0.f, 10.f, -15.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.0f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;

	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Camera_Default"), &CameraDesc))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Ready_Layer_Camera : Add_GameObjectToLayer(Prototype_GameObject_Camera_Default)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CPlayer* pAlphen = (CPlayer*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Alphen"));


	pAlphen->Set_PlayerPos(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	

	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	/*pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_Tutorial"));

	pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	pAlphen_TransformCom->Move(35.91f, 3.84f, 0.37f);
	pAlphen_TransformCom->Move(35.91f, pAlphen_TransformCom->Get_Height(pNavigation), 0.37f);
	pNavigation->Find_My_Cell(XMVectorSet(35.91f, 0.f, 0.37f, 1.f));*/
	//	break;
	//case Client::MAP_BALSELPH:

	//	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	//	pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_Balseph"));

	//	pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	//	pAlphen_TransformCom->Move(-75.31, 19.25, 0.02);
	//	pAlphen_TransformCom->Move(-75.31, pAlphen_TransformCom->Get_Height(pNavigation), 0.02);
	//	pNavigation->Find_My_Cell(XMVectorSet(-75.31, 19.25, 0.02, 1.f));
	//	break;
	//case Client::MAP_FIREAVATAR:

	//	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	//	pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_FireAvatar"));

	//	pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	//	pAlphen_TransformCom->Move(-91.22, 2.42, 0.32);
	//	pAlphen_TransformCom->Move(-91.22, pAlphen_TransformCom->Get_Height(pNavigation), 0.32);
	//	pNavigation->Find_My_Cell(XMVectorSet(-91.22, 2.42, 0.32, 1.f));
	//	break;
	//case Client::MAP_BATTLE02:

	//	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	//pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"));

	//pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	//pAlphen_TransformCom->Move(0.f, 0.f, 0.f);
	//pAlphen_TransformCom->Move(0.f, pAlphen_TransformCom->Get_Height(pNavigation), 0.f);
	//pNavigation->Find_My_Cell(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//	break;
	//case Client::MAP_BATTLE04:

	//	// 맵의 Navigation Mesh를 Alphen이 타게 함.
	//	pNavigation = m_pPlayerAlphen->SetUp_Navigation(TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_04"));

	//	pAlphen_TransformCom = dynamic_cast<CTransform*>(m_pPlayerAlphen->Get_Component(TEXT("Com_Transform")));
	//	pAlphen_TransformCom->Move(0.f, 0.f, 0.f);
	//	pAlphen_TransformCom->Move(0.f, pAlphen_TransformCom->Get_Height(pNavigation), 0.f);
	//	pNavigation->Find_My_Cell(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//	break;


	CBattle_Manager* pBattleManager = CBattle_Manager::GetInstance();

	 
	pBattleManager->Player_MoveToMap(pAlphen, _float3(1000.f, 0.f, 0.f), TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"));

	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Shionne")))
		return E_FAIL;



	//if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, pLayerTag, TEXT("Prototype_GameObject_Sword"), &SocketDesc))
	//	return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Map(const char * pModelFilePath, const char * pModelFileName, _vector vCenterPos)
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
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02_NaviMap"))
			) {
			Desc.eLevel = LEVEL_STATIC;
		}

		// 맵 오브젝트 생성
		CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Add_GameObjectToLayer(LEVEL_TEST, LAYER_MAP_TEST, TEXT("Prototype_GameObject_MapObject"), &Desc));
	}

	// 파일을 닫는다.
	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(L"Failed To CLevel_Test : Create : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();
	Safe_Release(m_pBattle_Manager);
}