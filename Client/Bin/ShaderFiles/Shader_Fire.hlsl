#include "Shader_Defines.hlsli"

cbuffer Matrix
{
	float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
}

cbuffer Socket
{
	float4x4 g_SocketMatrix;
}

cbuffer Noise
{
	float3 g_vScale = float3(1.f, 2.f, 3.f);
	float3 g_vScrollSpeed;
	float g_fFrame;
}

cbuffer Distortion
{
	float g_fDistortionScale;
	float g_fDistortionBias;
	float2 g_vDistortion1;
	float2 g_vDistortion2;
	float2 g_vDistortion3;
}

cbuffer Cam
{
	float g_fCamFar;
}

cbuffer Glow
{
	float g_OriginalStrength;
	float g_GlowStrength;
}

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD;
	float4		vProjPos : TEXCOORD1;
};

struct VS_OUT_FIRE
{
	float4 vPosition : SV_POSITION;
	float4 vProjPos : TEXCOORD0;
	float2 vTexUV : TEXCOORD1;
	float2 vTexCoord1 : TEXCOORD2;
	float2 vTexCoord2 : TEXCOORD3;
	float2 vTexCoord3 : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out;

	matrix matWV, matWVP, matWS, matWSV, matWSVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	matWS = mul(g_WorldMatrix, g_SocketMatrix);
	matWSV = mul(matWS, g_ViewMatrix);
	matWSVP = mul(matWSV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWSVP);
	Out.vProjPos = Out.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_FIRE VS_MAIN_FIRE(VS_IN In)
{
	VS_OUT_FIRE Out;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;
	Out.vTexUV = In.vTexUV;

	Out.vTexCoord1 = (In.vTexUV * g_vScale.x);
	Out.vTexCoord1.y = Out.vTexCoord1.y + (g_fFrame * g_vScrollSpeed.x);

	Out.vTexCoord2 = (In.vTexUV * g_vScale.y);
	Out.vTexCoord2.y = Out.vTexCoord2.y + (g_fFrame * g_vScrollSpeed.y);

	Out.vTexCoord3 = (In.vTexUV * g_vScale.z);
	Out.vTexCoord3.y = Out.vTexCoord3.y + (g_fFrame * g_vScrollSpeed.z);

	return Out;
}

texture2D g_ImageTexture;
texture2D g_NoiseTexture;
texture2D g_AlphaTexture;
texture2D g_DepthTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ClampSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD;
	float4		vProjPos : TEXCOORD1;
};

struct PS_IN_FIRE
{
	float4 vPosition : SV_POSITION;
	float4 vProjPos : TEXCOORD0;
	float2 vTexUV : TEXCOORD1;
	float2 vTexCoord1 : TEXCOORD2;
	float2 vTexCoord2 : TEXCOORD3;
	float2 vTexCoord3 : TEXCOORD4;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
	vector vBlur : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_ImageTexture.Sample(DefaultSampler, In.vTexUV);

	float fViewZ = In.vProjPos.w;
	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float fTargetViewZ = vDepthDesc.y * g_fCamFar;

	Out.vColor.a = max(min(fTargetViewZ - fViewZ, Out.vColor.a), 0.f);
	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vBlur = g_ImageTexture.Sample(DefaultSampler, In.vTexUV) * g_GlowStrength;

	float fViewZ = In.vProjPos.w;
	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float fTargetViewZ = vDepthDesc.y * g_fCamFar;

	Out.vBlur.a = max(min(fTargetViewZ - fViewZ, Out.vBlur.a), 0.f);
	return Out;
}

PS_OUT PS_MAIN_GLOW(PS_IN In)
{
	PS_OUT Out;
	Out.vColor = g_ImageTexture.Sample(DefaultSampler, In.vTexUV) * g_OriginalStrength;

	float fViewZ = In.vProjPos.w;
	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float fTargetViewZ = vDepthDesc.y * g_fCamFar;

	//Out.vColor.rgb *= 0.7f

	Out.vColor.a = max(min(fTargetViewZ - fViewZ, Out.vColor.a), 0.f);

	Out.vBlur = Out.vColor * g_GlowStrength;
	Out.vBlur.gb = 0;
	
	return Out;
}

PS_OUT PS_MAIN_GLOW_ALPHA(PS_IN In)
{
	PS_OUT Out;
	Out.vColor = g_ImageTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.rgb *= g_OriginalStrength;;

	float fViewZ = In.vProjPos.w;
	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float fTargetViewZ = vDepthDesc.y * g_fCamFar;


	Out.vColor.a = (Out.vColor.r + Out.vColor.g) * 0.5;

	if (Out.vColor.a <= 0.05)
		discard;

	Out.vColor.a = max(min(fTargetViewZ - fViewZ, Out.vColor.a), 0.f);

	Out.vBlur = Out.vColor * g_GlowStrength;
	Out.vBlur.gb = 0;

	return Out;
}

PS_OUT PS_MAIN_FIRE(PS_IN_FIRE In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float4 vNoise1, vNoise2, vNoise3, vFinalNoise;
	float4 vFireColor, vAlphaColor;
	float2 vNoiseCoord;
	float fPerturb;

	vNoise1 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord1);
	vNoise2 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord2);
	vNoise3 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord3);

	vNoise1 = (vNoise1 - 0.5f) * 2.f;
	vNoise2 = (vNoise2 - 0.5f) * 2.f;
	vNoise3 = (vNoise3 - 0.5f) * 2.f;

	vNoise1.xy *= g_vDistortion1.xy;
	vNoise2.xy *= g_vDistortion2.xy;
	vNoise3.xy *= g_vDistortion3.xy;

	vFinalNoise = vNoise1 + vNoise2 + vNoise3;
	fPerturb = ((1.f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	vNoiseCoord.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;

	vFireColor = g_ImageTexture.Sample(ClampSampler, vNoiseCoord);
	vAlphaColor = g_AlphaTexture.Sample(ClampSampler, vNoiseCoord);

	vFireColor.a = vAlphaColor;

	Out.vColor = vFireColor;

	float fViewZ = In.vProjPos.w;

	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float		fTargetViewZ = vDepthDesc.y * g_fCamFar;

	Out.vColor.a = max(min(fTargetViewZ - fViewZ, Out.vColor.a), 0.f);

	return Out;
}

PS_OUT PS_MAIN_FIRE_BLUR(PS_IN_FIRE In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float4 vNoise1, vNoise2, vNoise3, vFinalNoise;
	float4 vFireColor, vAlphaColor;
	float2 vNoiseCoord;
	float fPerturb;

	vNoise1 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord1);
	vNoise2 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord2);
	vNoise3 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord3);

	vNoise1 = (vNoise1 - 0.5f) * 2.f;
	vNoise2 = (vNoise2 - 0.5f) * 2.f;
	vNoise3 = (vNoise3 - 0.5f) * 2.f;

	vNoise1.xy *= g_vDistortion1.xy;
	vNoise2.xy *= g_vDistortion2.xy;
	vNoise3.xy *= g_vDistortion3.xy;

	vFinalNoise = vNoise1 + vNoise2 + vNoise3;
	fPerturb = ((1.f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	vNoiseCoord.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;

	vFireColor = g_ImageTexture.Sample(ClampSampler, vNoiseCoord);
	vAlphaColor = g_AlphaTexture.Sample(ClampSampler, vNoiseCoord);

	vFireColor.a = vAlphaColor;

	Out.vBlur = vFireColor;

	return Out;
}

PS_OUT PS_MAIN_FIRE_GLOW(PS_IN_FIRE In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float4 vNoise1, vNoise2, vNoise3, vFinalNoise;
	float4 vFireColor, vAlphaColor;
	float2 vNoiseCoord;
	float fPerturb;

	vNoise1 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord1);
	vNoise2 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord2);
	vNoise3 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord3);

	vNoise1 = (vNoise1 - 0.5f) * 2.f;
	vNoise2 = (vNoise2 - 0.5f) * 2.f;
	vNoise3 = (vNoise3 - 0.5f) * 2.f;

	vNoise1.xy *= g_vDistortion1.xy;
	vNoise2.xy *= g_vDistortion2.xy;
	vNoise3.xy *= g_vDistortion3.xy;

	vFinalNoise = vNoise1 + vNoise2 + vNoise3;
	fPerturb = ((1.f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	vNoiseCoord.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;

	vFireColor = g_ImageTexture.Sample(ClampSampler, vNoiseCoord);
	vAlphaColor = g_AlphaTexture.Sample(ClampSampler, vNoiseCoord);

	vFireColor.a = vAlphaColor;

	Out.vColor = vFireColor;
	Out.vBlur = vFireColor;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultRendering//0
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass BlurRendering//1
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass GlowRendering//2
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GLOW();
	}

	pass FireDefaultRendering//3
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_FIRE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FIRE();
	}

	pass FireBlurRendering//4
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_FIRE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FIRE_BLUR();
	}

	pass FireGlowRendering//5
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_FIRE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FIRE_GLOW();
	}

	pass FireEffectGlowRendering//6
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GLOW();
	}

	pass FireEffectGlowAlphaRendering//7
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GLOW_ALPHA();
	}
}