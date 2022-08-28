#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Battle_Manager.h"

BEGIN(Client)

class CMapObject;
class CWayPoint;
class CPlayer;
class CLord_Balseph;

END

BEGIN(Client)

class CLevel_Balseph final : public CLevel
{
private:
	explicit CLevel_Balseph(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual ~CLevel_Balseph() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Balseph(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Fire_Deco(const _tchar * pLayerTag);

	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);

private:
	HRESULT Ready_Map(const char * pModelFilePath, const char * pModelFileName, _vector vCenterPos);
	HRESULT Ready_WatPoint(LEVEL eNextLevel, _vector vPos);
	HRESULT Ready_WatPoint_Pos(_vector vPos, _vector vMovePos);

private:
	vector<CMapObject*>	m_vMapObject;	// 배치된 맵 오브젝트 vector
	vector<CWayPoint*>	m_vWayPoints;	// 배치된 맵 오브젝트 vector

public:
	CBattle_Manager* m_pBattle_Manager = nullptr;
	typedef struct tagUIInfo {
		_float fX;
		_float fY;
		_float fCX;
		_float fCY;

	}UIINFO;

private:
	CPlayer* m_pPlayerAlphen = nullptr;
	CLord_Balseph* m_pLord_Balseph = nullptr;

private:
	_tchar HitMsg[MAX_PATH] = TEXT("");
	_tchar DamageMsg[MAX_PATH] = TEXT("");
	_tchar StatusLine[MAX_PATH] = TEXT("");
	_tchar ComboHit[MAX_PATH] = TEXT("");
	_tchar DamageHit[MAX_PATH] = TEXT("");
	int combohitNum = 0;
	int damageNum = 0;
	bool m_isComboHit = false;
	bool m_isInvenOpen = false;
	bool m_isFirst = true;
	_float smashuiTime = 0;
	int next = 0;
	int LineMsgSprite = 1;
	bool m_isBattle = false;
	bool LineMsgCreate = false;
	bool m_isBattleOn = false;
	float fontpos = 0;
	bool m_isHit = false;
	float HitPos, DamagePos;
	bool m_bFirstBattleUI = false;
public:
	static CLevel_Balseph* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual void Free() override;
};

END