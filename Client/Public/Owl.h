#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CCollider;
class CRenderer;
class CModel;
class CNavigation;

END

BEGIN(Client)

class CPlayer;
class CUI_Owl;

class COwl final : public CGameObject {
public:
	enum OWL_ANIM_STATE {
		OWL_IN_POSITION_IDLE,						// 0000 제자리 - IDLE
		OWL_IN_POSITION_IDLE_LONG,					// 0001 제자리 - IDLE 길게

		OWL_IN_POSITION_OPEN_WING,					// 0031 제자리 - 날개 펼친상태 유지

		OWL_IN_POSITION_TILT_HEAD,					// 0060 제자리 - 고개 갸우뚱거리기

		OWL_IN_POSITION_WINGING_FAST,				// 0070 제자리 - 빠르게 날개짓

		OWL_IN_POSITION_WINGING_SLOW,				// 0080 제자리 - 천천히 날개짓

		OWL_IN_POSITION_LOOK_AROUND,				// 0090 제자리 - 여기저기 둘러보기

		OWL_IN_POSITION_FLAP_WINGS,					// 0100 제자리 - 날개 부르르 떨기

		OWL_IN_POSITION_BREATH_DEEP,				// 0120 제자리 - 숨 크게쉬었다 내뱉기

		OWL_IN_POSITION_LOSE_HEART,					// 0130 제자리 - 낙담하기
		OWL_IN_POSITION_LOSE_HEART_KEEP,			// 0131 제자리 - 낙담한 상태에서 고개 계속 숙이고 있기
		OWL_IN_POSITION_LOSE_HEART_RETURN,			// 0132 제자리 - 고개 숙인 상태에서 원상태로 복귀

		OWL_IN_POSITION_GLARE,						// 0160 제자리 - 위쪽 노려보기
		OWL_IN_POSITION_GLARE_KEEP,					// 0161 제자리 - 노려본 상태 지속
		OWL_IN_POSITION_GLARE_RETURN,				// 0162 제자리 - 노려보기다 원상태로 복귀

		OWL_IN_POSITION_TILT_BACK,					// 0170 제자리 - 고개 뒤로 젖히며 날개 펼치기
		OWL_IN_POSITION_TILT_BACK_KEEP,				// 0171 제자리 - 고개 뒤로 젖힌 상태 유지
		OWL_IN_POSITION_TILT_BACK_RETURN,			// 0172 제자리 - 고개 뒤로 젖힌 상태에서 원상태로 복귀

		OWL_IN_POSITION_DOZE,						// 0180 제자리 - 꾸벅꾸벅 졸기
		OWL_IN_POSITION_DOZE_KEEP,					// 0181 제자리 - 조는 상태 유지
		OWL_IN_POSITION_DOZE_RETURN,				// 0182 제자리 - 졸다 원상태로 복귀

		OWL_IN_POSITION_TO_FLYING,					// 0200 날개짓 - 제자리에서 날개짓해서 떠있기
		OWL_IN_FLYING_KEEP,							// 0201 날개짓 - 날개짓해서 떠있는 상태 유지
		OWL_IN_FLYING_TO_POSITION,					// 0202 날개짓 - 떠있다가 다시 원상태로 복귀

		OWL_IN_FLYING_TRUN_BACKWARD,				// 0210 날개짓 - 떠있는 상태에서 뒤로 한바퀴 돌기

		OWL_IN_FLYING_TRUN_SIDE_TO_SIDE,			// 0230 날개짓 - 떠있는 상태에서 좌우로 한바퀴 돌기

		OWL_IN_POSITION_TO_AWAY_DIAGONALLY,			// 0240 떠나기 - 제자리에서 대각선으로 날아가기

		OWL_IN_POSITION_TO_AWAY_DIAGONALLY_UP,		// 0250 떠나기 - 제자리에서 대각선으로 날아가다 위로 가기

		OWL_IN_POSITION_TO_AWAY_UP,					// 0260 떠나기 - 제자리에서 위로 날아가기
		OWL_END
	};

	/*
		기본 : 제자리 - IDLE
		조우시 : 제자리 - IDLE 길게

		
		상호작용하기 전부터, 상호작용한 다음에 나오는 것 2가지로 


		01 : 제자리 - 날개 펼친상태 유지
		02 : 
	
	
	
	*/



public:
	typedef struct _tagOwlDesc {
		_float4x4	TransformMatrix;	// 이동 행렬
		_float fDistance;	// 플레이어와의 거리
		const TCHAR* pTagItemName = nullptr;	// 아이템 이름
		const TCHAR* pModelTag = nullptr;	// 모델 이름
		CPlayer* pTargetPlayer = nullptr;	// 타겟이 되는 플레이어 정보
	}OwlDesc;

private:
	COwl(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	COwl(const COwl& rhs);
	virtual ~COwl() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static COwl* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	void Check_Collision(void);
	_bool m_bEncounter = false;

public:
	void Set_InRange(_bool bInRange) { m_bInRange = bInRange; }			// 범위 내에 플레이어가 있는지 확인하여 
	void Set_TargetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }	// 타겟이 되는 플레이어 설정


	void Check_InRange(void);

private:
	_uint m_iCurrentAnimationIndex = 0;
	_uint m_iNextAnimationIndex = 0;

	CPlayer* m_pPlayer = nullptr;		// 타겟이 되는 플레이어 정보
	_float m_fDistance;		// 타겟과의 거리 정보, 타겟과 거리가 해당 값보다 적을경우, 움직임을 시작한다.
	
	_bool m_bInRange = false;			// 타겟 플레이어가 일정 범위 내에 들어오면 true, 특정 애니메이션을 작동한다.

	_bool m_bInRoop = true;				// 플레이어가 x키를 누르기 전까지 일정 애니메이션을 반복한다. 누르고 나면 false가 되고, 부엉이가 날아간다.

	_bool m_bDone = false;				// 모든 상호작용을 끝낸 상태라면 true, 아니라면 false, 해당 변수가 true이면 더 이상 오브젝트가 작동하지 않는다.

	// 부엉이는 플레이어가 일정범위 내에 들어오면 하는 애니메이션과 상호작용시 애니메이션이 있다.

	// OWL_IN_POSITION_IDLE | OWL_IN_POSITION_IDLE_ACTIVE 상태로 가만히 있다가, 플레이어가 일정 범위 내에 들어오면 
	
	// OWL_IN_POSITION_TO_FLY -> OWL_IN_FLY_SPIN_AROUND -> OWL_IN_FLY_IDLE(IDLE 지속)
	// OWL_IN_POSITION_TILT_HEAD
	// OWL_IN_POSITION_FLAPPING
	
	// 위 3개중 하나의 행동을 한 뒤

	// OWL_IN_FLYAWAY_FORWARD_UPDOWN | OWL_IN_FLYAWAY_FORWARD_UPSTRAIGHT | OWL_IN_FLYAWAY_MOVEUP 중 하나의 애니메이션을 출력한다.
	// 이후, 오브젝트를 삭제하거나 출력하지 않는다.

	// 

private:
	const TCHAR* m_TagItemp = nullptr;		// 아이템 정보, 나중에 아이템 class를 만들고, 여기에 아이템 정보를 넣어준다. 우선 이름만 줘서 UI 상호작용에 사용.



private:
	_uint m_iOwlNumber = 14;

	CUI_Owl* m_pInteractionUI = nullptr;

};

END