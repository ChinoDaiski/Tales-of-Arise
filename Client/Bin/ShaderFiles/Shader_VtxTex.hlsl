#include "Shader_Defines.hlsli"

cbuffer Matrices
{
	float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
}

cbuffer UIBuffer
{
	float backuirgb;
	float glowbtnrgb;
	float emblemrgb;
	float smasheffectrgb;
	float tutolinemsgrgb;
	float alluirgb;
	float linemsgrgb;
}

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
	float4 vLocalPos : TEXCOORD1;
};

texture2D g_Texture;
texture2D g_DepthTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vLocalPos = vector(In.vPosition, 1.f);

	return Out;
}

struct VS_OUT_SOFT_EFFECT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
	float4 vProjPos : TEXCOORD1;
};

VS_OUT_SOFT_EFFECT VS_MAIN_SOFT_EFFECT(VS_IN In)
{
	VS_OUT_SOFT_EFFECT Out;

	float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	vector vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vLocalPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
	vector vBlur : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}

PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}

struct PS_IN_SOFT_EFFECT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
	float4 vProjPos : TEXCOORD1;
};

//SmashEffect를 위한 쉐이더
PS_OUT PS_SMASHEFFECTUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = smasheffectrgb;
	if (Out.vColor.a < 0.2f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_SOFT_EFFECT(PS_IN_SOFT_EFFECT In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	float fViewZ = In.vProjPos.w;
	float2 vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	float fTargetViewZ = vDepthDesc.y * 300.f;

	Out.vColor.a = max(min(fTargetViewZ - fViewZ, Out.vColor.a), 0.f);
	return Out;
}

PS_OUT PS_BackUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = backuirgb;
	return Out;
}

cbuffer Percentage
{
	float g_fPercentage;
	bool m_isRed;
}

PS_OUT PS_CPPOINT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (-0.5f + g_fPercentage <= In.vLocalPos.y)
	{
		discard;
	}
	if (m_isRed)
	{
		Out.vColor.r = 1;
		Out.vColor.g = 0;
		Out.vColor.b = 0;
	}
	else
	{
		Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	}
	return Out;
}

//emblem을 위한 쉐이더
PS_OUT PS_MAINSEMBLEM(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r <= 0.1f && Out.vColor.g <= 0.1 && Out.vColor.b <= 0.1)
	{
		discard;
	}

	Out.vColor.a = emblemrgb;
	return Out;
}

//exUI 뒷배경을 위한 쉐이더
PS_OUT PS_EXUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r < 0.05)
	{
		discard;
	}
	return Out;
}

//Line을 위한 쉐이더
PS_OUT PS_MAINLINEUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r < 0.35f && Out.vColor.g < 0.35f&& Out.vColor.b < 0.35f)
	{
		discard;
	}
	return Out;
}

//HPBar 를 위한 쉐이더
PS_OUT PS_MAINHPBar(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r > 0.01)
	{
		discard;
	}
	return Out;
}

//InfoFadeUI를 위하.ㄴ
PS_OUT PS_MAINInfoFadeUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r < 0.01)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAINHPUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r >= 1 && Out.vColor.g >= 0.5&& Out.vColor.b >= 0.5)
	{
		discard;
	}
	return Out;
}

PS_OUT PAALLUIRGB(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = alluirgb;
	return Out;
}

//LineMsgUI를 위한.
PS_OUT PS_LINEMSGUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = linemsgrgb;
	return Out;
}

//디졸브
PS_OUT PSDISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r < Out.vColor.r - 0.4)
	{
		discard;
	}
	return Out;
}

PS_OUT TUTOLINEMSG(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = tutolinemsgrgb;
	return Out;
}

PS_OUT LINEMSG2(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = 0.7;
	return Out;
}

//LineMsgUI를 위한.
PS_OUT PS_STATUSUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.r <= 0.002f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_Picking_Rect(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}

cbuffer HORIZONTALPERCENT
{
	float horizontalpercent;
	float g_Redbar;
	bool time;
}

PS_OUT PS_MAIN_PERCENTAGE_HORRIZON(PS_IN In) {
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.r == 0)
	{
		if (horizontalpercent > In.vTexUV.x)
		{
			Out.vColor.r = 0.888;
			Out.vColor.g = 1;
			Out.vColor.b = 0.3;
		}
		else if (g_Redbar > In.vTexUV.x)
		{
			Out.vColor.r = 1;
			Out.vColor.g = 0;
			Out.vColor.b = 0;
		}
	}
	return Out;
}

cbuffer againHP
{
	float lefthpPercent;
	float g_leftRedBar;
}

PS_OUT LEFTHPPERCENT(PS_IN In) {
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.r == 0)
	{
		if (lefthpPercent > In.vTexUV.x)
		{
			Out.vColor.r = 0.888;
			Out.vColor.g = 1;
			Out.vColor.b = 0.3;
		}
		else if (g_leftRedBar > In.vTexUV.x)
		{
			Out.vColor.r = 1;
			Out.vColor.g = 0;
			Out.vColor.b = 0;
		}
	}
	return Out;
}

cbuffer Alpha
{
	float g_fAlpha;
}

PS_OUT PS_OWL_UI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a *= g_fAlpha;
	return Out;
}

cbuffer Dissolve
{
	vector g_vDissolveColor;
	float g_fDissolveTime;
}

struct VS_OUT2
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
};

VS_OUT2 VS_MAIN2(VS_IN In)
{
	VS_OUT2	Out;

	float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

texture2D g_SourDissolveTexture;
texture2D g_DestDissolveTexture;

struct PS_IN2
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
};

PS_OUT PS_MAIN_DISSOLVE(PS_IN2 In)
{
	PS_OUT Out = (PS_OUT)0.f;
	vector vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	vector vSourDissolveColor = g_SourDissolveTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDestDissolveColor = g_DestDissolveTexture.Sample(DefaultSampler, In.vTexUV);

	float fDissolve = 1.f - g_fDissolveTime;

	float fWidth = 0.08f;

	if (vSourDissolveColor.r <= fDissolve)
	{
		vColor.a = 1.f;
	}
	else
	{
		discard;
	}

	if (vSourDissolveColor.r - fWidth <= fDissolve && vSourDissolveColor.r + fWidth >= fDissolve)
	{
		vColor.rgb = vDestDissolveColor.rgb;
	}
	if (vSourDissolveColor.r + 1.3f * fWidth >= fDissolve && vSourDissolveColor.r + fWidth < fDissolve)
	{
		vColor.rgb *= 0.4;
	}
	Out.vColor = vColor;
	return Out;
}

struct PS_OUT_DISTORTION
{
	float4 vColor : SV_TARGET2;
};

PS_OUT_DISTORTION PS_MAIN_DISTORTIONTRAIL(PS_IN In)
{
	PS_OUT_DISTORTION Out = (PS_OUT_DISTORTION)0.f;
	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


cbuffer HORIZONTALPERCENTMonster
{
	float Monsterhoripercent;
	float g_MonsterRedbar;
}

PS_OUT PS_MAIN_PERCENTAGE_HORRIZONMONSTER(PS_IN In) {
	PS_OUT Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);


	if (Out.vColor.r == 0)
	{

		//이거 그거 오렌지색.
		//Out.vColor.r = 0.882;
		//Out.vColor.g = 0.792;
		//Out.vColor.b = 0.376;

		//이민희추가
		if (Monsterhoripercent > In.vTexUV.x)
		{
			//Out.vColor.r = 0.96;
			//Out.vColor.g = 0.74;
			//Out.vColor.b = 0.23;

			Out.vColor.r = 0.882;
			Out.vColor.g = 0.792;
			Out.vColor.b = 0.376;
		}
		else if (g_MonsterRedbar > In.vTexUV.x)
		{
			Out.vColor.r = 0.92;
			Out.vColor.g = 0.36;
			Out.vColor.b = 0.176;
		}
	}
	return Out;
}

//다이아몬드를 위한 쉐이더

cbuffer diamondPercent
{
	float g_fDiamondPercent;
	bool m_isBlue;
}
PS_OUT PS_DIAMONDAG(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.r = 0.031;
	Out.vColor.g = 0.882;
	Out.vColor.b = 0.882;
	if (-0.5f + g_fDiamondPercent <= In.vLocalPos.x)
	{
		discard;
	}

	if (m_isBlue)
	{
		Out.vColor.r = 0.031;
		Out.vColor.g = 0.882;
		Out.vColor.b = 0.882;
	}
	else
	{

		Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	}

	return Out;
}

cbuffer PlayerMovingHP
{
	float playermovingpercent;
	float playerrendbar;
	bool g_isMonster;
}
//26
PS_OUT PlayerMovingHpPass(PS_IN In) {
	PS_OUT Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);


	if (Out.vColor.r == 0)
	{
		if (playermovingpercent > In.vTexUV.x)
		{


			Out.vColor.r = 0.888f;
			Out.vColor.g = 1.f;
			Out.vColor.b = 0.3f;

			//	if (!g_isMonster) {
			//		/*Out.vColor.r = 0.888f;
			//		Out.vColor.g = 1.f;
			//		Out.vColor.b = 0.3f;
			//*/
			//	}
			//else {
			//	Out.vColor.r = 0.933f;
			//	Out.vColor.g = 0.819f;
			//	Out.vColor.b = 0.407f;
			//
			//}
		}
		else if (playerrendbar > In.vTexUV.x)
		{
			//빨간색
			Out.vColor.r = 0.839f;
			Out.vColor.g = 0.270f;
			Out.vColor.b = 0.027f;

		}
	}


	return Out;

}

cbuffer MonsterMovingHP
{
	float Monstermovingpercent;
	float Monsterendbar;
	bool g_ismonst;
}
//27
PS_OUT MonsterMovingHP(PS_IN In) {
	PS_OUT Out;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);


	if (Out.vColor.r == 0)
	{
		if (Monstermovingpercent > In.vTexUV.x)
		{


			Out.vColor.r = 0.933f;
			Out.vColor.g = 0.819f;
			Out.vColor.b = 0.407f;


		}
		else if (Monsterendbar > In.vTexUV.x)
		{
			//빨간색
			Out.vColor.r = 0.839f;
			Out.vColor.g = 0.270f;
			Out.vColor.b = 0.027f;

		}
	}


	return Out;

}
technique11 DefaultTechnique
{
	pass DefaultRendering //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass SoftEffect	//1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOFT_EFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SOFT_EFFECT();
	}

	pass BackUI	//2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOFT_EFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BackUI();
	}
	pass DefaultAlphaBlending //3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass MouseAlphaBlending //4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass EMBLEM //5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAINSEMBLEM();
	}

	pass EXUI //6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EXUI();
	}

	pass UISmasheffect //7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMASHEFFECTUI();
	}

	pass Line //8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAINLINEUI();
	}

	pass HPBar //9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAINHPBar();
	}

	pass HPPUI //10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAINHPUI();
	}

	pass INFOFADEUI //11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAINInfoFadeUI();
	}

	pass LineMSGUI //12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LINEMSGUI();
	}

	pass Dissolve //13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PSDISSOLVE();
	}

	pass NonAlpha //14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass TutoLIneMsg //15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 TUTOLINEMSG();
	}

	pass AllUIDA //16
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PAALLUIRGB();
	}

	pass LINEMSG2 //17
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 LINEMSG2();
	}

	pass statusUI //18
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_STATUSUI();
	}

	pass CPPoint //19
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CPPOINT();
	}

	pass Navi_Picking_Rect  //20
	{
		SetRasterizerState(RS_WireFrame);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Picking_Rect();
	}

	pass HORIZONTALPASS  //21
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PERCENTAGE_HORRIZON();
	}

	pass LEFTHPPERCENTPASS  //22
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 LEFTHPPERCENT();
	}

	pass Owl_UI  // 23
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OWL_UI();
	}

	pass SceneDissolve  // 24
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN2();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass DistortionTrail  //25
	{
		SetRasterizerState(RS_FrontBack);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_NonBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTIONTRAIL();
	}

	pass HORIZONTALMONSTER  	//26
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PERCENTAGE_HORRIZONMONSTER();
	}

	pass DIAMONDAGPASS 	//27
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DIAMONDAG();
	}

	pass PlayerMovingHppass2 	//28
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PlayerMovingHpPass();
	}

	pass MonsterMovingPass 	//29
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 MonsterMovingHP();
	}
}