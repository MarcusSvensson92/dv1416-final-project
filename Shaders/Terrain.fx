cbuffer cbPerFrame
{
	matrix gWorld;
	matrix gWorldViewProj;
	float3 gCameraPosition;

	float3 gTargetPosition;
};

RasterizerState wireframeRS
{
	FillMode = Wireframe;
};

SamplerState linearSampler
{
	Filter   = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VSIn
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 tex0     : TEX0;
};

struct PSIn
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW   : NORMAL;
	float2 tex0		 : TEX0;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;
	output.positionH = mul(float4(input.position, 1.f), gWorldViewProj);
	output.positionW = mul(input.position, (float3x3)gWorld);
	output.normalW	 = mul(input.normal, (float3x3)gWorld);
	output.tex0		 = input.tex0;
	return output;
}

float4 PS(PSIn input) : SV_TARGET
{
	float offset = 0.5f;
	
	//if (input.positionW.x <= gTargetPosition.x + offset &&
	//	input.positionW.x >= gTargetPosition.x - offset &&
	//	input.positionW.z <= gTargetPosition.z + offset &&
	//	input.positionW.z >= gTargetPosition.z - offset)
	//	return float4(1.f, 0.f, 0.f, 1.f);

	float3 distanceVector = gTargetPosition - input.positionW;	
	float d = length(distanceVector);
	if( d < offset )
		return float4(1.f, 0.f, 0.f, 1.f);
	else
		return float4(0.f, 0.f, 0.f, 1.f);
}

technique11 RenderTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(wireframeRS);
	}
}