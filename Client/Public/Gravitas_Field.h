#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CGravitas_Field final : public CBullet
{
public:
	typedef struct tagBulletDESC
	{
		class CModel*	pModelCom = nullptr;
		const char*		pBoneName = nullptr;	//뼈이름
		_vector			vTargetPos;				//몬스터위치

	}BULLETDESC;

private:
	explicit CGravitas_Field(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CGravitas_Field(const CGravitas_Field& rhs);
	virtual ~CGravitas_Field() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;


private:
	BULLETDESC			m_BulletDesc;
	_float4x4*			m_pSocketMatrix;
	_float4x4			m_PivotMatrix;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	_double m_Time = 0.0;
	_double m_BackTime = 0.0;


	class CMeshEffect* m_pSmallSphere = nullptr;
	class CMeshEffect* m_pBigSphere = nullptr;
	class CMeshEffect* m_pSmallSphere2 = nullptr;
	class CMeshEffect* m_pBigSphere2 = nullptr;


	class CMeshEffect* m_pWave1 = nullptr;
	class CMeshEffect* m_pWave2 = nullptr;
	class CMeshEffect* m_pWave3 = nullptr;
	class CMeshEffect* m_pWave4 = nullptr;

	class CRect_Effect* m_pParticle1 = nullptr;
	class CRect_Effect* m_pParticle2 = nullptr;
	class CRect_Effect* m_pParticle3 = nullptr;
	class CRect_Effect* m_pParticle4 = nullptr; //발사
	class CRect_Effect* m_pParticle5 = nullptr; //날라갈떄
	class CRect_Effect* m_pParticle6 = nullptr; //터질때
	class CRect_Effect* m_pParticle7 = nullptr;  //구체내부

	_bool m_bMakeBullet = false;
	_bool m_bFireBullet = false;
	_bool m_bOnce = false;
	_double m_dAliveTime = 0.0;

public:
	static CGravitas_Field* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

