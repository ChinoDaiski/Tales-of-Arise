#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"
#include "VIBuffer_Rect_Instance.h"

BEGIN(Client)

class CFireAvatar_InstanceRect final : public CBlendObject
{
public:
	typedef struct tagFireInstanceDesc
	{
		EFFECTDESC* pEffectDesc;
		CTransform* pTransformCom;
		CModel* pSocketModel;
		const char* tagBone;
		_float3 vRevisePos;
	}FIREINSTANCEDESC;
private:
	CFireAvatar_InstanceRect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFireAvatar_InstanceRect(const CFireAvatar_InstanceRect& rhs);
	virtual ~CFireAvatar_InstanceRect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
public:
	EFFECTDESC*	Get_CreateDesc() { return m_EffectDesc; }
	_bool Get_Finish() { return m_bFinish; }

	void Set_Pos(_float4 Pos) { m_EffectDesc->vPostion = Pos; }
	void Set_TimeSpeed(_float TimeSpeed) { m_fmultipleTime = TimeSpeed; }
	void Reset_PassTime() { m_PassTime = m_EffectDesc->fPassTime; }
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT BufferSet();
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect_Instance* m_pVIBufferCom = nullptr;
private:
	FIREINSTANCEDESC m_tFireInstanceDesc;
private:
	_bool m_bFinish = false;
	_float m_fMaxTime = 10.f;
	_float m_PassTime = 0;
	_float m_fmultipleTime = 1.f;
	EFFECTDESC* m_EffectDesc = nullptr;
private:
	_float4x4* m_pSocketMatrix = nullptr;
public:
	static CFireAvatar_InstanceRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END