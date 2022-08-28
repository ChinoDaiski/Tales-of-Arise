
#include "Shader_Defines.hlsli"

cbuffer Matrices
{
	float4x4      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
	float4x4      g_SocketMatrix;
}

texture2D   g_DiffuseTexture;
texture2D   g_DissolveTexture;
texture2D   g_MaskTexture;
//texture2D   g_Texture;

float      g_UVMax;
float      g_Dissolve;
float      g_SubUV;
float3      g_Shader;
float4      g_Color1;
float4      g_Color2;

float2      g_Blur;


sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state {
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ClampSampler = sampler_state {
	filter = min_mag_mip_point;
	AddressU = clamp;
	AddressV = clamp;
};

struct VS_IN
{
	float3      vPosition : POSITION;
	float2      vTexUV : TEXCOORD;
};


struct VS_OUT_MODEL
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
	float2      vRealUV : TEXCOORD1;
};

VS_OUT_MODEL VS_MAIN_MODEL(VS_IN In)
{
	VS_OUT_MODEL      Out;

	float4      vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vRealUV = In.vTexUV;
	In.vTexUV.x += g_Shader.x;
	In.vTexUV.y += g_Shader.y;
	Out.vTexUV = In.vTexUV;

	return Out;
}


VS_OUT_MODEL VS_MAIN_MODEL_SOCKET(VS_IN In)
{
	VS_OUT_MODEL      Out;

	float4      vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vRealUV = In.vTexUV;
	In.vTexUV.x += g_Shader.x;
	In.vTexUV.y += g_Shader.y;
	Out.vTexUV = In.vTexUV;

	return Out;
}


struct PS_IN_MODEL
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
	float2      vRealUV : TEXCOORD1;
};

struct PS_OUT
{
	vector         vDiffuse : SV_TARGET0;
	vector         vBlur : SV_TARGET1;
};

PS_OUT PS_MAIN_MODEL(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


	if (vMtrlDiffuse.a == 0.f)
		discard;
	else
	{
		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}


	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT PS_BLACKCUT(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

	if (vMtrlDiffuse.a <= 0.1f)
		discard;
	else
	{
		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	return Out;
}

PS_OUT PS_MAIN_MODEL_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


		if (vMtrlDiffuse.a == 0.f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;


		if (vMtrlDiffuse.a <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_UV(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_UVx(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.x);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_UVy(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.y);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}


PS_OUT PS_BLACKCUT_UV_x(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_UV_y(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_UVMX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.x = In.vTexUV.x*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_UVMY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.y = In.vTexUV.y*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		Out.vDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		Out.vDiffuse.a = Out.vDiffuse.a * g_Shader.z* vDissolve.r;
		Out.vDiffuse = Out.vDiffuse * g_Blur.x;

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			Out.vBlur = Out.vDiffuse * g_Blur.y;

			if (Out.vDiffuse.a == 0.f)
				discard;
		}


	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_MASK_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;

			if (Out.vDiffuse.a == 0.f)
				discard;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_UVMXY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV = In.vTexUV*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT PS_BLACKCUT_UV_x_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_UV_y_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}
PS_OUT PS_BLACKCUT_DISSOLVE_UVMX_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax)
	{
		In.vTexUV.x = In.vTexUV.x*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}

		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_UVMY_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		In.vTexUV.y = In.vTexUV.y*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}

		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;
		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_DISSOLVE_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse;
			Out.vBlur = vMtrlDiffuse;

			if (Out.vDiffuse.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT_MASK_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse;
			Out.vBlur = vMtrlDiffuse;

			if (Out.vDiffuse.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}
//PS_OUT PS_BLACKCUT_MASK_RGB(PS_IN_MODEL In)
//{
//   PS_OUT         Out = (PS_OUT)0;
//
//   vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);
//   vector      vMtrlDiffuse1 = g_DiffuseTexture.Sample(DefaultSampler, In.vRealUV + In.vTexUV);
//   vector      vMtrlDiffuse2 = g_DiffuseTexture.Sample(DefaultSampler, (In.vRealUV * 2) + In.vTexUV);
//   vector      vMtrlDiffuse3 = g_DiffuseTexture.Sample(DefaultSampler, (In.vRealUV * 3) + In.vTexUV);
//   
//
//   float a = vDissolve.r *(vMtrlDiffuse1.r*0.33 + vMtrlDiffuse2.r*0.33 + vMtrlDiffuse3.r*0.33);
//   vMtrlDiffuse1 = (vMtrlDiffuse1.r*g_Color1) + ((1.f - vMtrlDiffuse1.r) * g_Color2);
//   vMtrlDiffuse1.a = a*vMtrlDiffuse1.a;
//
//   if (vMtrlDiffuse1.a <= g_Dissolve)
//      discard;
//   else
//   {
//      Out.vDiffuse = vMtrlDiffuse * g_Blur.x1;
//      Out.vBlur = vMtrlDiffuse * g_Blur.y1;
//   }
//   return Out;
//}

PS_OUT NONEBLUR_PS_MAIN_MODEL(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


	if (vMtrlDiffuse.a == 0.f)
		discard;
	else
	{
		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

	}


	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;


	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

	if (vMtrlDiffuse.a <= 0.1f)
		discard;
	else
	{
		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

	}
	return Out;
}

PS_OUT NONEBLUR_PS_MAIN_MODEL_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


		if (vMtrlDiffuse.a == 0.f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;


		if (vMtrlDiffuse.a <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}

	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UV(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UVx(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.x);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UVy(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.y);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}


PS_OUT NONEBLUR_PS_BLACKCUT_UV_x(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UV_y(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.x = In.vTexUV.x*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;


	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.y = In.vTexUV.y*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;


	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		Out.vDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		Out.vDiffuse.a = Out.vDiffuse.a * g_Shader.z* vDissolve.r;
		Out.vDiffuse = Out.vDiffuse * g_Blur.x;

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			if (Out.vDiffuse.a == 0.f)
				discard;
		}


	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_MASK_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

			if (Out.vDiffuse.a == 0.f)
				discard;
		}

	}
	else
		discard;

	return Out;
}


PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMXY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV = In.vTexUV*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}

	Out.vDiffuse = vMtrlDiffuse * g_Blur.x;


	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UV_x_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_UV_y_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;
			Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

		}
	}
	else
		discard;

	return Out;
}
PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMX_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax)
	{
		In.vTexUV.x = In.vTexUV.x*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}

		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMY_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		In.vTexUV.y = In.vTexUV.y*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}

		Out.vDiffuse = vMtrlDiffuse * g_Blur.x;

	}
	else
		discard;

	return Out;
}
PS_OUT NONEBLUR_PS_BLACKCUT_DISSOLVE_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse;


			if (Out.vDiffuse.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT NONEBLUR_PS_BLACKCUT_MASK_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vDiffuse = vMtrlDiffuse;


			if (Out.vDiffuse.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}
//////////////////////////////////////////////////////////////
PS_OUT NONEDIFFUSE_PS_MAIN_MODEL(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


	if (vMtrlDiffuse.a == 0.f)
		discard;
	else
	{

		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}


	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}


	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
	vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

	if (vMtrlDiffuse.a <= 0.1f)
		discard;
	else
	{

		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	return Out;
}

PS_OUT NONEDIFFUSE_PS_MAIN_MODEL_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Color1.a * g_Shader.z;


		if (vMtrlDiffuse.a == 0.f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UV(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;


		if (vMtrlDiffuse.a <= g_Dissolve)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UV(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UVx(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.x);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UVy(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(In.vRealUV.y);

		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}


PS_OUT NONEDIFFUSE_PS_BLACKCUT_UV_x(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UV_y(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.x = In.vTexUV.x*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}


	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV.y = In.vTexUV.y*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}


	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		Out.vBlur = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		Out.vBlur.a = Out.vDiffuse.a * g_Shader.z* vDissolve.r;
		Out.vBlur = Out.vDiffuse * g_Blur.y;

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			Out.vBlur = Out.vBlur * g_Blur.y;

			if (Out.vBlur.a == 0.f)
				discard;
		}


	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_MASK_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{

			Out.vBlur = vMtrlDiffuse * g_Blur.y;

			if (Out.vDiffuse.a == 0.f)
				discard;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMXY(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	In.vTexUV = In.vTexUV*g_UVMax;
	vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

	if (vMtrlDiffuse.r <= g_Dissolve)
		discard;
	else
	{
		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
	}


	Out.vBlur = vMtrlDiffuse * g_Blur.y;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UV_x_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);


		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* (1.f - In.vRealUV.x);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.x));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_UV_y_NOA(PS_IN_MODEL In)
{

	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);

		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z *(1.f - In.vRealUV.y);
		//vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z*(1 - (In.vTexUV.y));
		if (vMtrlDiffuse.a <= 0.05f)
			discard;
		else
		{
			vMtrlDiffuse.a = 1.f;

			Out.vBlur = vMtrlDiffuse * g_Blur.y;
		}
	}
	else
		discard;

	return Out;
}
PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMX_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax)
	{
		In.vTexUV.x = In.vTexUV.x*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}


		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMY_TEX(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;
	if (In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		In.vTexUV.y = In.vTexUV.y*g_SubUV;
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		//vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV);

		if (vMtrlDiffuse.r <= g_Dissolve)
			discard;
		else
		{
			vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
			vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z;
		}


		Out.vBlur = vMtrlDiffuse * g_Blur.y;
	}
	else
		discard;

	return Out;
}
PS_OUT NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vBlur = vMtrlDiffuse;


			if (Out.vBlur.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}

PS_OUT NONEDIFFUSE_PS_BLACKCUT_MASK_TEX_PP(PS_IN_MODEL In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (In.vTexUV.x > -g_UVMax && In.vTexUV.x < g_UVMax && In.vTexUV.y > -g_UVMax && In.vTexUV.y < g_UVMax)
	{
		vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
		vector      vDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vRealUV);


		vMtrlDiffuse = (vMtrlDiffuse.r*g_Color1) + ((1.f - vMtrlDiffuse.r) * g_Color2);
		vMtrlDiffuse.a = vMtrlDiffuse.a * g_Shader.z* vDissolve.r;


		if (vDissolve.r <= g_Dissolve)
			discard;
		else
		{
			Out.vBlur = vMtrlDiffuse;


			if (Out.vBlur.a <= 0.01f)
				discard;
		}

	}
	else
		discard;

	return Out;
}
technique11 DefaultTechnique
{
	pass Model//0
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MODEL();
	}

	pass Model1//1
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE();
	}

	pass Model2//2
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT();
	}

	pass Model3//3
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MODEL_UV();
	}

	pass Model4//4
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UV();
	}

	pass Model5//5
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UV();
	}
	pass Model6//6
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UVx();
	}
	pass Model7//7
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UVy();
	}
	pass Model8//8
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UV_x();
	}
	pass Model9//9
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UV_y();
	}
	pass Model10//10
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UVMX();
	}
	pass Model11//11
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UVMY();
	}
	pass Model12//12
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_TEX();
	}
	pass Model13//13
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_MASK_TEX();
	}
	pass Model14//14
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UVMXY();
	}
	pass Model15//15
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UV_x_NOA();
	}
	pass Model16//16
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_UV_y_NOA();
	}
	pass Model17//17
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UVMX_TEX();
	}
	pass Model18//18
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_UVMY_TEX();
	}
	pass Model19//19
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_DISSOLVE_TEX_PP();
	}
	pass Model20//20
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT_MASK_TEX_PP();
	}
	////////////////////////////////////////////
	pass Mode1_NoneBlur//0
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_MAIN_MODEL();
	}

	pass Model1_NoneBlur//1
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE();
	}

	pass Model2_NoneBlur//2
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT();
	}

	pass Model3_NoneBlur//3
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_MAIN_MODEL_UV();
	}

	pass Model4_NoneBlur//4
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UV();
	}

	pass Model5_NoneBlur//5
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UV();
	}
	pass Model6_NoneBlur//6
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UVx();
	}
	pass Model7_NoneBlur//7
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UVy();
	}
	pass Model8_NoneBlur//8
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UV_x();
	}
	pass Model9_NoneBlur//9
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UV_y();
	}
	pass Model10_NoneBlur//10
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMX();
	}
	pass Model11_NoneBlur//11
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMY();
	}
	pass Model12_NoneBlur//12
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_TEX();
	}
	pass Model13_NoneBlur//13
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_MASK_TEX();
	}
	pass Model14_NoneBlur//14
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMXY();
	}
	pass Model15_NoneBlur//15
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UV_x_NOA();
	}
	pass Model16_NoneBlur//16
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_UV_y_NOA();
	}
	pass Model17_NoneBlur//17
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMX_TEX();
	}
	pass Model18_NoneBlur//18
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_UVMY_TEX();
	}
	pass Model19_NoneBlur//19
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_DISSOLVE_TEX_PP();
	}
	pass Model20_NoneBlur//20
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEBLUR_PS_BLACKCUT_MASK_TEX_PP();
	}
	////////////////////////////////////////////////////////////

	pass Mode1_NoneDiffuse//0
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_MAIN_MODEL();
	}

	pass Model1_NoneDiffuse//1
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE();
	}

	pass Model2_NoneDiffuse//2
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT();
	}

	pass Model3_NoneDiffuse//3
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_MAIN_MODEL_UV();
	}

	pass Model4_NoneDiffuse//4
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_MODEL();
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UV();
	}

	pass Model5_NoneDiffuse//5
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UV();
	}
	pass Model6_NoneDiffuse//6
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UVx();
	}
	pass Model7_NoneDiffuse//7
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UVy();
	}
	pass Model8_NoneDiffuse//8
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UV_x();
	}
	pass Model9_NoneDiffuse//9
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UV_y();
	}
	pass Model10_NoneDiffuse//10
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMX();
	}
	pass Model11_NoneDiffuse//11
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMY();
	}
	pass Model12_NoneDiffuse//12
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_TEX();
	}
	pass Model13_NoneDiffuse//13
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_MASK_TEX();
	}
	pass Model14_NoneDiffuse//14
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMXY();
	}
	pass Model15_NoneDiffuse//15
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UV_x_NOA();
	}
	pass Model16_NoneDiffuse//16
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_UV_y_NOA();
	}
	pass Model17_NoneDiffuse//17
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMX_TEX();
	}
	pass Model18_NoneDiffuse//18
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_UVMY_TEX();
	}
	pass Model19_NoneDiffuse//19
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_DISSOLVE_TEX_PP();
	}
	pass Model20_NoneDiffuse//20
	{
		SetRasterizerState(RS_NonCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MODEL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 NONEDIFFUSE_PS_BLACKCUT_MASK_TEX_PP();
	}
}