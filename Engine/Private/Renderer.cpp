#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "BlendObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PIpeLine.h"
#include "PostProcessing.h"

#ifdef _DEBUG
bool g_bWireFrame = false;
bool g_bDistortion = false;
bool g_bDeferred = true;
bool g_bLightAcc = true;
bool g_bLightDepth = true;
bool g_bFinal = true;
bool g_bHDR = true;
bool g_bEffect = true;
#endif // _DEBUG

CRenderer::CRenderer(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CComponent(pDeviceOut, pDeviceContextOut)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : nullptr == m_pTarget_Manager", E_FAIL);

	_uint iNumViewports = 1;
	D3D11_VIEWPORT Viewport;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Create_DSV(m_pDevice, g_iLightCX, g_iLightCY)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Create_DSV", E_FAIL);

	/* For.Target_Output */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Output"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 1.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Output)", E_FAIL);
	/* For.Target_Dissolve */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Dissolve"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 1.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Output)", E_FAIL);
	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Diffuse)", E_FAIL);
	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Normal)", E_FAIL);
	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Depth)", E_FAIL);
	/* For.Target_Emissive */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Emissive"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Emissive)", E_FAIL);
	/* For.Target_ObjectGlow */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_ObjectGlow"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_ObjectGlow)", E_FAIL);
	/* For.Target_NonBlur */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_NonBlur"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_NonBlur)", E_FAIL);
	/* For.Target_Blur */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Blur"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Blur)", E_FAIL);
	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Shade)", E_FAIL);
	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Specular)", E_FAIL);
	/* For.Target_LightDepth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_LightDepth"), m_pDevice, m_pDeviceContext, g_iLightCX, g_iLightCY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_LightDepth)", E_FAIL);
	/* For.Target_HDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_HDR"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_HDR)", E_FAIL);
	/* For.Target_Distortion */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Distortion"), m_pDevice, m_pDeviceContext, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_RenderTarget(Target_Distortion)", E_FAIL);

	/* For.MRT_Final */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Output"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Final)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Dissolve"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Final)", E_FAIL);

	/* For.MRT_Deferred */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Deferred)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Deferred)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Deferred)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Deferred)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_ObjectGlow"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Deferred)", E_FAIL);

	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_LightAcc)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_LightAcc)", E_FAIL);

	/* For.MRT_LightDepth */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_LightDepth"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_LightDepth)", E_FAIL);

	/* For.MRT_HDR */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_HDR)", E_FAIL);

	/* For.MRT_Effect */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_NonBlur"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Effect)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Blur"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Effect)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Distortion"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Add_MRT(MRT_Effect)", E_FAIL);

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : nullptr == m_pVIBuffer", E_FAIL);

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : nullptr == m_pShader", E_FAIL);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());	

	m_WorldMatrix._11 = Viewport.Width;
	m_WorldMatrix._22 = Viewport.Height;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));


#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugBuffer", E_FAIL);
	
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Diffuse"), 0.f, 0.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Diffuse)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Normal"), 0.f, 150.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Normal)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Depth"), 0.f, 300.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Depth)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Emissive"), 0.f, 450.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Emissive)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_ObjectGlow"), 0.f, 600.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_ObjectGlow)", E_FAIL);

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shade"), 150.f, 0.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Shade)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Specular"), 150.f, 150.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Specular)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_LightDepth"), 150.f, 300.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_LightDepth)", E_FAIL);

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_HDR"), Viewport.Width - 150.f, 0.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_HDR)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Output"), Viewport.Width - 150.f, 150.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Output)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Dissolve"), Viewport.Width - 150.f, 300.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Dissolve)", E_FAIL);

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_NonBlur"), Viewport.Width - 300.f, 0.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_NonBlur)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Blur"), Viewport.Width - 300.f, 150.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Blur)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Distortion"), Viewport.Width - 300.f, 300.f, 150.f, 150.f)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : Ready_DebugDesc(Target_Distortion)", E_FAIL);

	if (FAILED(SetUp_RenderState()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : NativeConstruct_Prototype : SetUp_RenderState", E_FAIL);
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::SetUp_RenderState()
{
	D3D11_RASTERIZER_DESC tWireDesc;
	ZeroMemory(&tWireDesc, sizeof(D3D11_RASTERIZER_DESC));

	tWireDesc.FillMode = D3D11_FILL_WIREFRAME;
	tWireDesc.CullMode = D3D11_CULL_NONE;
	m_pDevice->CreateRasterizerState(&tWireDesc, &m_pWireFrame);

	D3D11_RASTERIZER_DESC tDefault;
	ZeroMemory(&tDefault, sizeof(D3D11_RASTERIZER_DESC));

	tDefault.FillMode = D3D11_FILL_SOLID;
	tDefault.FrontCounterClockwise = FALSE;
	tDefault.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&tDefault, &m_pRasterDefault);
	return S_OK;
}

#else
HRESULT CRenderer::SetUp_RenderState()
{
	D3D11_RASTERIZER_DESC tDefault;
	ZeroMemory(&tDefault, sizeof(D3D11_RASTERIZER_DESC));

	tDefault.FillMode = D3D11_FILL_SOLID;
	tDefault.FrontCounterClockwise = FALSE;
	tDefault.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&tDefault, &m_pRasterDefault);
	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::NativeConstruct(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Add_RenderGroup : nullptr == pGameObject", E_FAIL);

	m_RenderList[eGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);
	return S_OK;
}

HRESULT CRenderer::Render_GameObjects()
{
	if(FAILED(m_pTarget_Manager->Clear_MRT(m_pDeviceContext, L"MRT_HDR")))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Clear_MRT(MRT_HDR)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Clear_MRT(m_pDeviceContext, L"MRT_Effect")))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Clear_MRT(MRT_Effect)", E_FAIL);
	if (FAILED(m_pTarget_Manager->Clear_RenderTarget(L"Target_Output")))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Clear_RenderTarget(Target_Output)", E_FAIL);

	if (FAILED(Render_Priority()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Priority", E_FAIL);
	if (FAILED(Render_LightDepth()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_LightDepth", E_FAIL);
	if (FAILED(Render_NonBlend()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_NonBlend", E_FAIL);
	if (FAILED(Render_LightAcc()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_LightAcc", E_FAIL);
	if (FAILED(Render_Deferred()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Deferred", E_FAIL);
	if (FAILED(Render_NonLight()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_NonLight", E_FAIL);
	if (FAILED(Render_Blend()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Blend", E_FAIL);
	if (FAILED(Render_Result()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Result", E_FAIL);
	if (FAILED(Render_Output()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Output", E_FAIL);
	if (FAILED(Render_UI()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_UI", E_FAIL);

#ifdef _DEBUG

	if (FAILED(Render_DebugComponents()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_DebugComponents", E_FAIL);
	
	if (true == g_bDeferred)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_Deferred)", E_FAIL);
	}
	if (true == g_bLightAcc)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_LightAcc)", E_FAIL);
	}
	if (true == g_bLightDepth)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_LightDepth)", E_FAIL);
	}
	if (true == g_bFinal)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_Final"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_Final)", E_FAIL);
	}
	if (true == g_bHDR)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_HDR"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_HDR)", E_FAIL);
	}
	if (true == g_bEffect)
	{
		if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_Effect"))))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_GameObjects : Render_Debug(MRT_Effect)", E_FAIL);
	}
	
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);
	return S_OK;
}
#endif // _DEBUG

_bool CRenderer::Get_DissolveStart()
{
	return m_bDissolveStart;
}

void CRenderer::Set_DissolveStart(_bool bStart)
{
	m_bDissolveStart = bStart;
}

HRESULT CRenderer::Render_Priority()
{	
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Priority : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Priority : Begin_NonClear_MRT(MRT_HDR)", E_FAIL);

	HRESULT hr = 0;
	
	for (auto& pGameObject : m_RenderList[GROUP_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			hr = pGameObject->Render();
			Safe_Release(pGameObject);
		}
		if (FAILED(hr))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Priority : hr", E_FAIL);
	}
	m_RenderList[GROUP_PRIORITY].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Priority : End_MRT(MRT_HDR)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Terrain()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Terrain : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Terrain : Begin_NonClear_MRT(MRT_HDR)", E_FAIL);

	HRESULT hr = 0;

	for (auto& pGameObject : m_RenderList[GROUP_TERRAIN])
	{
		if (nullptr != pGameObject)
		{
			hr = pGameObject->Render();
			Safe_Release(pGameObject);
		}
		if (FAILED(hr))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Terrain : hr", E_FAIL);
	}
	m_RenderList[GROUP_TERRAIN].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Terrain : End_MRT(MRT_HDR)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_LightDepth()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightDepth : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_LightDepth_MRT(m_pDeviceContext, TEXT("MRT_LightDepth"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightDepth : Begin_LightDepth_MRT(MRT_LightDepth)", E_FAIL);

	HRESULT hr = 0;

	for (auto& pGameObject : m_RenderList[GROUP_LIGHTDEPTH])
	{
		if (nullptr != pGameObject)
		{
			hr = pGameObject->Render_LightDepth();
			Safe_Release(pGameObject);
		}
		if (FAILED(hr))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightDepth : hr", E_FAIL);
	}
	m_RenderList[GROUP_LIGHTDEPTH].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_LightDepth"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightDepth : End_MRT(MRT_LightDepth)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : Begin_MRT(MRT_Deferred)", E_FAIL);

	HRESULT hr = 0;

	for (auto& pGameObject : m_RenderList[GROUP_NONBLEND])
	{
		if (nullptr != pGameObject)
		{
#ifdef _DEBUG
			if (true == g_bWireFrame)
			{
				m_pDeviceContext->RSSetState(m_pWireFrame);
			}
			else
			{
				m_pDeviceContext->RSSetState(m_pRasterDefault);
			}
#else
			m_pDeviceContext->RSSetState(m_pRasterDefault);
#endif // _DEBUG
			hr = pGameObject->Render();
			Safe_Release(pGameObject);
		}
		if (FAILED(hr))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : hr", E_FAIL);
	}
	m_RenderList[GROUP_NONBLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : End_MRT(MRT_Deferred)", E_FAIL);

	if (FAILED(Blur_Emissive()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : Blur_Emissive", E_FAIL);

	if (FAILED(Glow_Object()))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonBlend : Glow_Object", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Blur_Emissive()
{
	ID3D11RenderTargetView* pRTV = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
	m_pDeviceContext->OMGetRenderTargets(1, &pRTV, &pDSV);

	ID3D11RenderTargetView* pRTVNullptr = nullptr;
	m_pDeviceContext->OMSetRenderTargets(1, &pRTVNullptr, nullptr);

	CPostProcessing* pPostProcessing = GET_INSTANCE(CPostProcessing);
	if (FAILED(pPostProcessing->DownScale_Blur(m_pTarget_Manager->Get_SRV(L"Target_Emissive"), CPostProcessing::EMISSIVE)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Blur_Emissive : DownScale_Blur", E_FAIL);

	RELEASE_INSTANCE(CPostProcessing);

	m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
	return S_OK;
}

HRESULT CRenderer::Glow_Object()
{
	ID3D11RenderTargetView* pRTV = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
	m_pDeviceContext->OMGetRenderTargets(1, &pRTV, &pDSV);

	ID3D11RenderTargetView* pRTVNullptr = nullptr;
	m_pDeviceContext->OMSetRenderTargets(1, &pRTVNullptr, nullptr);

	CPostProcessing* pPostProcessing = GET_INSTANCE(CPostProcessing);
	if (FAILED(pPostProcessing->DownScale_Blur(m_pTarget_Manager->Get_SRV(L"Target_ObjectGlow"), CPostProcessing::OBJECTGLOW)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Glow_Object : DownScale_Blur", E_FAIL);

	RELEASE_INSTANCE(CPostProcessing);

	m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager || nullptr == m_pLight_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightAcc : nullptr", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightAcc : Begin_MRT(MRT_LightAcc)", E_FAIL);

	m_pLight_Manager->Render_Lights();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_LightAcc : End_MRT(MRT_LightAcc)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Begin_NonClear_MRT(MRT_HDR)", E_FAIL);

	CPostProcessing* pPostProcessing = GET_INSTANCE(CPostProcessing);

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_DiffuseTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_ShadeTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_SpecularTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_DepthTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_LightDepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_LightDepth")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_LightDepthTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_EmissiveTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_EmissiveBlurTexture", pPostProcessing->Get_DownScale4x4SRV(CPostProcessing::EMISSIVE))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_EmissiveBlurTexture)", E_FAIL);
	if (FAILED(m_pShader->Set_ShaderResourceView("g_GlowTexture", pPostProcessing->Get_DownScale4x4SRV(CPostProcessing::OBJECTGLOW))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_ShaderResourceView(g_GlowTexture)", E_FAIL);

	RELEASE_INSTANCE(CPostProcessing);

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_WorldMatrix)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_ProjMatrix)", E_FAIL);

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4 ViewMatrixInv, ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_ViewMatrixInv)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_ProjMatrixInv)", E_FAIL);

	if (FAILED(m_pShader->Set_RawValue("g_LightViewMatrix", &pPipeLine->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_LightViewMatrix)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_LightProjMatrix", &pPipeLine->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_LightProjMatrix)", E_FAIL);

	_float fFar = pPipeLine->Get_CamFar();
	if (FAILED(m_pShader->Set_RawValue("g_fCamFar", &fFar, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_fCamFar)", E_FAIL);
	fFar = pPipeLine->Get_LightFar();
	if (FAILED(m_pShader->Set_RawValue("g_fLightFar", &fFar, sizeof(_float))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : Set_RawValue(g_fLightFar)", E_FAIL);

	RELEASE_INSTANCE(CPipeLine);

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Deferred : End_MRT(MRT_HDR)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonLight : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_Effect"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonLight : Begin_NonClear_MRT(MRT_Effect)", E_FAIL);

	m_RenderList[GROUP_NONLIGHT].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return ((CBlendObject*)pSour)->Get_CamDistance() < ((CBlendObject*)pDest)->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderList[GROUP_NONLIGHT])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonLight : Render", E_FAIL);

			Safe_Release(pGameObject);
		}
	}
	m_RenderList[GROUP_NONLIGHT].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Effect"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_NonLight : End_MRT(MRT_Effect)", E_FAIL);

	return S_OK;

}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Blend : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_Effect"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Blend : Begin_NonClear_MRT(MRT_Effect)", E_FAIL);

	HRESULT hr = 0;

	m_RenderList[GROUP_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderList[GROUP_ALPHABLEND])
	{
		if (nullptr != pGameObject)
		{
			hr = pGameObject->Render();
			Safe_Release(pGameObject);
		}

		if (FAILED(hr))
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Blend : hr", E_FAIL);
	}

	m_RenderList[GROUP_ALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Effect"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Blend : End_MRT(MRT_Effect)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Result()
{
	if (nullptr == m_pTarget_Manager)
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Result : nullptr == m_pTarget_Manager", E_FAIL);

	if (FAILED(m_pTarget_Manager->Begin_NonClear_MRT(m_pDeviceContext, TEXT("MRT_Final"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Result : Begin_NonClear_MRT(MRT_Final)", E_FAIL);

	CPostProcessing* pPostProcessing = GET_INSTANCE(CPostProcessing);
	pPostProcessing->Set_DissolveStart(m_bDissolveStart);

	_float4x4 Matrix[3] = { m_WorldMatrix, m_ViewMatrix, m_ProjMatrix };

	if (FAILED(pPostProcessing->Combine_Resources(m_pVIBuffer, Matrix)))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Result : Combine_Resources", E_FAIL);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Final"))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Result : End_MRT(MRT_Final)", E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Output()
{
	if (FAILED(m_pShader->Set_ShaderResourceView("g_Texture", m_pTarget_Manager->Get_SRV(TEXT("Target_Output")))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Output : Set_ShaderResourceView(g_Texture)", E_FAIL);

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Output : Set_RawValue(g_WorldMatrix)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Output : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CRenderer : Render_Output : Set_RawValue(g_ProjMatrix)", E_FAIL);

	m_pShader->Begin(0);
	m_pVIBuffer->Render();
	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	HRESULT hr = 0;
	m_RenderList[GROUP_UI].sort([](CGameObject* pSourIter, CGameObject* pDestIter) {
		return pSourIter->Get_DistanceToCamera() < pDestIter->Get_DistanceToCamera();
	});

	for (auto& pGameObject : m_RenderList[GROUP_UI])
	{
		if (nullptr != pGameObject)
		{
			hr = pGameObject->Render();
			Safe_Release(pGameObject);
		}
		if (FAILED(hr))
		{
			MSG_CHECK_RETURN(L"Failed To CRenderer : Render_UI : hr", E_FAIL);
		}
	}
	m_RenderList[GROUP_UI].clear();
	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_DebugComponents()
{
	for (auto& pComponent : m_DebugComponents)
	{
		if (nullptr != pComponent)
		{
			pComponent->Render();
			Safe_Release(pComponent);
		}				
	}
	m_DebugComponents.clear();
	return S_OK;
}
#endif // _DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To CRenderer : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();
	return this;
}

void CRenderer::Free()
{
	__super::Free();
	for (_uint i = 0; i < GROUP_END; ++i)
	{
		for (auto& pGameObject : m_RenderList[i])
		{
			Safe_Release(pGameObject);
		}
		m_RenderList[i].clear();
	}
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

#ifdef _DEBUG
	Safe_Release(m_pWireFrame);
#endif // _DEBUG
	Safe_Release(m_pRasterDefault);
}