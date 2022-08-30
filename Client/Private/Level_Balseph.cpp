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
#include "Sky.h"
#include "Fire_Deco.h"

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

	// 오브젝트 정보 추가
	if (FAILED(Ready_Map("../Bin/Resources/Dat/Stage03/", "MeshList.dat", XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// 빛 정보 추가
	if (FAILED(Ready_Lights("../Bin/Resources/Dat/Stage03/", "LightInfo.dat")))
		return E_FAIL;

	// 스카이 박스 정보 추가
	if (FAILED(Ready_Skybox("../Bin/Resources/Dat/Stage03/", "SkyboxInfo.dat")))
		return E_FAIL;

	// 불 정보 추가
	if (FAILED(Ready_MapFire("../Bin/Resources/Dat/Stage03/", "FireInfo.dat", LEVEL_LORD_BALSEPH)))
		return E_FAIL;

	// 포스트 프로세싱 정보 추가
	if (FAILED(Ready_PostProcessing("../Bin/Resources/Dat/Stage03/", "PostProcessingInfo.dat")))
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


	SetWindowText(g_hWnd, TEXT("Boss1"));

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

HRESULT CLevel_Balseph::Ready_Lights(const char * pModelFilePath, const char * pModelFileName)
{
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 기존에 존재하는 빛의 정보를 삭제한다.
	pGameInstance->Clear_Lights();

	while (true)
	{
		// 방향광의 갯수 읽기
		_uint iDirectionalLightCnt;
		ReadFile(hFile, &iDirectionalLightCnt, sizeof(_uint), &dwByte, nullptr);

		LIGHTDESC LightDesc;
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		for (int i = 0; i < iDirectionalLightCnt; ++i) {
			// 방향광의 빛 정보 로드 
			ReadFile(hFile, &LightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);

			// 방향광 생성
			pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc);
		}


		// 점광원의 갯수 읽기
		_uint iPointLightCnt;
		ReadFile(hFile, &iPointLightCnt, sizeof(_uint), &dwByte, nullptr);

		// 점광원의 정보 로드
		for (int i = 0; i < iPointLightCnt; ++i) {
			// 점광원의 빛 정보 로드 
			ReadFile(hFile, &LightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);

			// 점광원 생성
			pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc);
		}

		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	// 파일을 닫는다.
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_Skybox(const char * pModelFilePath, const char * pModelFileName)
{
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

	// 현재 스카이 박스 정보 불러오기
	_uint iCurTextureNum = 0;
	ReadFile(hFile, &iCurTextureNum, sizeof(_uint), &dwByte, nullptr);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CSky* m_pSkybox = dynamic_cast<CSky*>(pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, LAYER_MAPTOOL_Skybox, TEXT("Prototype_GameObject_Sky")));

	if (nullptr == m_pSkybox)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	m_pSkybox->Set_CurTextureNum(iCurTextureNum);

	// 파일을 닫는다.
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_MapFire(const char * pModelFilePath, const char * pModelFileName, LEVEL eLevel)
{
	// 파일을 읽어 오브젝트와 불의 정보를 가져온다.
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CMapObject*> vMapObjects;
	vector<CFire_Deco*> FireInfo[MAX_STANDARD_OBJECT_CNT];

	while (true)
	{
		_uint iStandard_Object_Cnt = 0;
		ReadFile(hFile, &iStandard_Object_Cnt, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iStandard_Object_Cnt; ++i) {
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

			// 읽어온 오브젝트 생성
			CMapObject::MAPOBJDesc Desc;
			Desc.pPrototype_ObjectName = pName;

			XMStoreFloat4x4(&Desc.TransformMatrix, XMMatrixIdentity());

			// 충돌체 생성
			Desc.fColliderSize = _float3(10.f, 10.f, 10.f);

			// 레벨 설정
			Desc.eLevel = LEVEL_STATIC;

			// 맵 오브젝트 생성
			CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Clone_Prototype(TEXT("Prototype_GameObject_MapObject"), &Desc));
			vMapObjects.push_back(pMapObject);		// 맵 오브젝트 정보 푸쉬

			_uint iFireCnt = 0;		// 불 갯수 읽기
			ReadFile(hFile, &iFireCnt, sizeof(_uint), &dwByte, nullptr);

			// 불의 갯수만큼 반복하여 불의 정보를 읽어오기
			CFire_Deco::FIREDECODESC FireDesc;
			for (_uint j = 0; j < iFireCnt; ++j) {
				ReadFile(hFile, &FireDesc, sizeof(CFire_Deco::FIREDECODESC), &dwByte, nullptr);

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

				FireDesc.tagTextureCom = pName;

				// 읽은 정보를 사용하여 불을 생성
				CFire_Deco* pFire = dynamic_cast<CFire_Deco*>(pGameInstance->Clone_Prototype(TEXT("Prototype_GameObject_Fire"), &FireDesc));
				FireInfo[i].push_back(pFire);
			}
		}

		break;
	}

	// 파일을 닫는다.
	CloseHandle(hFile);





	// 읽은 불 정보를 맵에 배치되어 있는 오브젝트를 돌며 관련 오브젝트가 존재할 경우 해당 오브젝트에 불을 생성한다.
	CLayer* pLayer = pGameInstance->Find_Layer(eLevel, LAYER_MAPTOOL_PlacedObjects);

	if (nullptr == pLayer)
		return E_FAIL;

	auto& Objlist = pLayer->Get_ObjectList();

	_uint iMapObjectCnt = 0;
	for (auto& MapObject : vMapObjects) {
		// 파일에서 가져온 모델 태그 정보
		const TCHAR* pMapObjectModelTag = dynamic_cast<CMapObject*>(MapObject)->Get_ModelTag();

		// 맵에 설치된 오브젝트 리스트 정보
		for (auto& Obj : Objlist) {
			// 설치된 오브젝트의 모델 Tag 정보를 가져온다.
			const TCHAR* pObjModelTag = dynamic_cast<CMapObject*>(Obj)->Get_ModelTag();

			// 가져온 모델의 이름과 파일에서 가져온 모델 태그 정보를 비교한다.
			if (!_tcscmp(pMapObjectModelTag, pObjModelTag)) {
				// 만약 같은 정보가 존재할 경우
				// 해당 오브젝트가 가지고 있는 불 정보를 가져와 맵에 설치된 오브젝트의 위치에 소환한다.

				// 맵에 설치되어 있는 오브젝트의 행렬 정보를 가져온다.
				CTransform* pTransform = dynamic_cast<CTransform*>(Obj->Get_Component(TEXT("Com_Transform")));
				_matrix matMapObjectWorld = pTransform->Get_WorldMatrix();

				for (auto& Fire : FireInfo[iMapObjectCnt]) {
					CFire_Deco::FIREDECODESC FireDesc;
					FireDesc.fGlowStrength = Fire->Get_GlowStrength();
					FireDesc.fOriginalStrength = Fire->Get_OriginalStrength();
					FireDesc.fRotation = Fire->Get_Rotation();
					FireDesc.iFrameSpeed = Fire->Get_FrameSpeed();
					FireDesc.iShaderPass = 2;

					CTransform* pFireTransform = dynamic_cast<CTransform*>(Fire->Get_Component(TEXT("Com_Transform")));

					XMStoreFloat3(&FireDesc.vPosition, pFireTransform->Get_State(CTransform::STATE_POSITION));
					FireDesc.vScale = pFireTransform->Get_Scale();
					FireDesc.tagTextureCom = TEXT("Prototype_Component_Texture_Fire0");
					FireDesc.vLook = _float3(0.f, 0.f, 1.f);

					// 불을 생성하는데 필요한 정보 저장
					WriteFile(hFile, &FireDesc, sizeof(CFire_Deco::FIREDECODESC), &dwByte, nullptr);

					// 불 생성
					CFire_Deco* pFire = dynamic_cast<CFire_Deco*>(pGameInstance->Add_GameObjectToLayer(eLevel, LAYER_MAP_FIRE, TEXT("Prototype_GameObject_Fire"), &FireDesc));

					// 불 위치 조정
					CTransform* pFire_Transform = dynamic_cast<CTransform*>(pFire->Get_Component(TEXT("Com_Transform")));
					_matrix matFireWorld = pFire_Transform->Get_WorldMatrix();

					matFireWorld = XMMatrixMultiply(matFireWorld, matMapObjectWorld);
					pFire_Transform->Set_WorldMatrix(matFireWorld);

					_float4x4 matFireWorld4X4;
					XMStoreFloat4x4(&matFireWorld4X4, matFireWorld);


					// 카메라 검색
					CLayer* pCameraLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Camera"));
					CCamera* pCurrCamera = nullptr;

					// 카메라가 존재할 경우
					if (nullptr != pCameraLayer) {
						pCurrCamera = dynamic_cast<CCamera*>(*pCameraLayer->Get_ObjectList().begin());

						// 불 정보에 카메라를 세팅한다.
						pFire->Set_Target(pCurrCamera);
					}
				}
			}
		}
		++iMapObjectCnt;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Balseph::Ready_PostProcessing(const char * pModelFilePath, const char * pModelFileName)
{
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// MiddleGray 저장
	_float fMiddleGray = 0.f;
	ReadFile(hFile, &fMiddleGray, sizeof(_float), &dwByte, nullptr);
	pGameInstance->Set_MiddleGray(fMiddleGray);

	// White 저장
	_float fWhite = 0.f;
	ReadFile(hFile, &fWhite, sizeof(_float), &dwByte, nullptr);
	pGameInstance->Set_White(fWhite);

	RELEASE_INSTANCE(CGameInstance);

	// 파일을 닫는다.
	CloseHandle(hFile);

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