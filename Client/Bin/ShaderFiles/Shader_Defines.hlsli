RasterizerState	RS_Default
{
	FillMode = Solid;
	FrontCounterClockwise = false;
	CullMode = back;
};

RasterizerState	RS_NonCull
{
	FillMode = Solid;
	FrontCounterClockwise = false;
	CullMode = none;
};

RasterizerState	RS_WireFrame
{
	FillMode = WireFrame;
};

RasterizerState	RS_Cull_CW
{
	FillMode = Solid;
	FrontCounterClockwise = false;
	CullMode = front;
};

RasterizerState	RS_FrontBack
{
	FillMode = Solid;
	FrontCounterClockwise = false;
	CullMode = none;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_NonZTestAndWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DSS_NonZTest
{
	DepthEnable = false;
	DepthWriteMask = 1;
};

BlendState BS_NonBlend
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};
