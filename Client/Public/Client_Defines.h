#pragma once

namespace Client
{
	const unsigned int	g_iWinCX = 1920;//1280; // 1280;
	const unsigned int	g_iWinCY = 1080;//720; // 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_LOBBY, LEVEL_TUTORIAL, LEVEL_LORD_BALSEPH, LEVEL_FIRE_AVATAR, LEVEL_MapTool, LEVEL_TEST, LEVEL_END };

	enum MAP_TYPE { MAP_GYT, MAP_BALSELPH, MAP_FIREAVATAR, MAP_BATTLE02, MAP_BATTLE04, MAP_END };

	enum DAMAGE_TYPE { DAMAGE_TYPE_DOWN, DAMAGE_TYPE_LARGE, DAMAGE_TYPE_SMALL, DAMAGE_TYPE_END };

	enum DAMAGE_DIR { DAMAGE_DIR_FRONT, DAMAGE_DIR_BACK, DAMAGE_DIR_LEFT, DAMAGE_DIR_RIGHT, DAMAGE_DIR_END };

}

#include <process.h>

#define  LAYER_MAPTOOL_CAMERA					TEXT("Layer_MapTool_Camera")
#define  LAYER_MAPTOOL_TERRAIN					TEXT("Layer_MapTool_Terrain")
#define  LAYER_MAPTOOL_BattleMap				TEXT("Layer_MapTool_BattleMap")
#define  LAYER_MAPTOOL_PlacedObjects			TEXT("Layer_MapTool_PlacedObjects")
#define  LAYER_MAPTOOL_Owls						TEXT("Layer_MapTool_Owls")
#define  LAYER_MAPTOOL_WayPoints				TEXT("Layer_MapTool_WayPoints")
#define  LAYER_MAPTOOL_Balseph_Stair			TEXT("Layer_MapTool_Balseph_Stair")
#define  LAYER_MAPTOOL_Skybox					TEXT("Layer_MapTool_Skybox")
#define  LAYER_WayPoints						TEXT("Layer_WayPoints")
#define  LAYER_MAPTOOL_Picking_Rect				TEXT("Layer_MapTool_PickingB_Rect")
#define  LAYER_MAPTOOL_Dot						TEXT("Layer_MapTool_Dot")

#define  LAYER_MAP_BATTLE						TEXT("Layer_Map_Battle")
#define  LAYER_MAP_TEST							TEXT("Layer_Map_Test")
#define  LAYER_MAP_FIRE							TEXT("Layer_Map_Fire")


// 로드할 수 있는 기준이 되는 오브젝트의 최대 갯수 - fire와 관련이 있는 변수
#define MAX_STANDARD_OBJECT_CNT	10


extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
