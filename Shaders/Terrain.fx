#include		"Light.fx"

cbuffer cbPerFrame
{
	matrix gWorld;
	matrix gWorldViewProj;
	float3 gCameraPosition;

	float3 gTargetPosition;

	bool gUseBlendmap;
};

cbuffer cbPerObject
{
	Material gMaterial;
}

cbuffer cbConstant
{
	float gTextureScale = 5.f;
};

Texture2D	   gBlendmap;
Texture2DArray gLayermapArray;

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
	float2 tiledTex0 : TEX1;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;
	output.positionH = mul(float4(input.position, 1.f), gWorldViewProj);
	output.positionW = mul(input.position, (float3x3)gWorld);
	output.normalW	 = mul(input.normal, (float3x3)gWorld);
	output.tex0		 = input.tex0;
	output.tiledTex0 = input.tex0 * gTextureScale;
	return output;
}

float4 PS(PSIn input) : SV_TARGET
{
	float offset = 1.f;
	
	if (input.positionW.x <= gTargetPosition.x + offset &&
		input.positionW.x >= gTargetPosition.x - offset &&
		input.positionW.z <= gTargetPosition.z + offset &&
		input.positionW.z >= gTargetPosition.z - offset)
		return float4(1.f, 0.f, 0.f, 1.f);

	/*float3 distanceVector = gTargetPosition - input.positionW;
	float d = length(distanceVector);
	if( d < offset )
		return float4(1.f, 0.f, 0.f, 1.f);*/

	if (gUseBlendmap)
	{
		float4 c0 = gLayermapArray.Sample(linearSampler, float3(input.tiledTex0, 0.f));
		float4 c1 = gLayermapArray.Sample(linearSampler, float3(input.tiledTex0, 1.f));
		float4 c2 = gLayermapArray.Sample(linearSampler, float3(input.tiledTex0, 2.f));
		float4 c3 = gLayermapArray.Sample(linearSampler, float3(input.tiledTex0, 3.f));
		float4 c4 = gLayermapArray.Sample(linearSampler, float3(input.tiledTex0, 4.f));

		float4 t = gBlendmap.Sample(linearSampler, input.tex0);

		float4 texColor = c0;
		texColor = lerp(texColor, c1, t.r);
		texColor = lerp(texColor, c2, t.g);
		texColor = lerp(texColor, c3, t.b);
		texColor = lerp(texColor, c4, t.a);

		// The toEye vector is used in lighting.
		float3 toEye = gCameraPosition - input.positionW;
		// Cache the distance to the eye from this surface point.
		float distToEye = length(toEye); 
		// Normalize.
		toEye /= distToEye;
		// Lighting.
		float4 litColor = texColor;
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		[loop]
		for( uint i = 0;i < POINTLIGHTS; i++ )
		{
			float4 A,D,S;
			ComputePointLight(gMaterial, gPointLights[i], input.positionW, input.normalW, toEye, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}

		// Modulate with late add.
		litColor = texColor*(ambient + diffuse) + spec;
		litColor.a = gMaterial.Diffuse.a * texColor.a;
		return litColor;
	}

	return float4(1.f, 1.f, 1.f, 1.f);
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