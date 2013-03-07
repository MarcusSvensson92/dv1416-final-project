#include		"Light.fx"
#include		"Shadow.fx"

cbuffer cbPerFrame
{
	matrix gWorld;
	matrix gViewProj;
	float3 gCameraPosition;

	float gMinDistance;
	float gMaxDistance;
	float gMinTessellation;
	float gMaxTessellation;

	float2 gTexelSize;

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

Texture2D	   gHeightmap;
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

SamplerState heightmapSampler
{
	Filter   = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VSIn
{
	float3 position : POSITION;
	float2 tex0     : TEX0;
};

struct HSIn
{
	float3 positionW : POSITION;
	float2 tex0		 : TEX0;
};


HSIn VS(VSIn input)
{
	HSIn output;

	output.positionW   = input.position;
	output.positionW.y = gHeightmap.SampleLevel(heightmapSampler, input.tex0, 0).r;
	output.positionW   = mul(output.positionW, (float3x3)gWorld);

	output.tex0		 = input.tex0;

	return output;
}

float computeTessellationFactor(float3 p)
{
	float d = distance(p, gCameraPosition);
	float s = saturate((d - gMinDistance) / (gMaxDistance - gMinDistance));
	return pow(2, lerp(gMaxTessellation, gMinTessellation, s));
}

struct TessellationPatch
{
	float edges[4]	 : SV_TessFactor;
	float insides[2] : SV_InsideTessFactor;
};

TessellationPatch ConstantHS(InputPatch<HSIn, 4> patch, uint patchID : SV_PrimitiveID)
{
	float3 edge0  = 0.5f  * (patch[0].positionW + patch[2].positionW);
	float3 edge1  = 0.5f  * (patch[0].positionW + patch[1].positionW);
	float3 edge2  = 0.5f  * (patch[1].positionW + patch[3].positionW);
	float3 edge3  = 0.5f  * (patch[2].positionW + patch[3].positionW);
	float3 center = 0.25f * (patch[0].positionW + patch[1].positionW + patch[2].positionW + patch[3].positionW);

	TessellationPatch output;
	output.edges[0]   = computeTessellationFactor(edge0);
	output.edges[1]   = computeTessellationFactor(edge1);
	output.edges[2]   = computeTessellationFactor(edge2);
	output.edges[3]	  = computeTessellationFactor(edge3);
	output.insides[0] = computeTessellationFactor(center);
	output.insides[1] = output.insides[0];
	return output;
}

struct DSIn
{
	float3 positionW : POSITION;
	float2 tex0		 : TEX0;
};

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
DSIn HS(InputPatch<HSIn, 4> patch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	DSIn output;

	output.positionW = patch[i].positionW;
	output.tex0		 = patch[i].tex0;

	return output;
}

struct PSIn
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex0		 : TEX0;
	float2 tiledTex0 : TEX1;
};

[domain("quad")]
PSIn DS(TessellationPatch tp, float2 uv : SV_DomainLocation, const OutputPatch<DSIn, 4> patch)
{
	PSIn output;

	output.positionW = lerp(lerp(patch[0].positionW, patch[1].positionW, uv.x),
							lerp(patch[2].positionW, patch[3].positionW, uv.x),
							uv.y);
	output.tex0		 = lerp(lerp(patch[0].tex0, patch[1].tex0, uv.x),
							lerp(patch[2].tex0, patch[3].tex0, uv.x),
							uv.y);
	output.tiledTex0 = output.tex0 * gTextureScale;

	output.positionW.y = gHeightmap.SampleLevel(heightmapSampler, output.tex0, 0).r;

	output.positionH = mul(float4(output.positionW, 1.f), gViewProj);

	return output;
}

float4 PS(PSIn input) : SV_TARGET
{
	// Estimate normal
	float2 leftTex0   = input.tex0 + float2(-gTexelSize.x, 0.f);
	float2 rightTex0  = input.tex0 + float2( gTexelSize.x, 0.f);
	float2 bottomTex0 = input.tex0 + float2(0.f,  gTexelSize.y);
	float2 topTex0	  = input.tex0 + float2(0.f, -gTexelSize.y);

	float leftY   = gHeightmap.SampleLevel(heightmapSampler, leftTex0,	 0).r;
	float rightY  = gHeightmap.SampleLevel(heightmapSampler, rightTex0,  0).r;
	float bottomY = gHeightmap.SampleLevel(heightmapSampler, bottomTex0, 0).r;
	float topY	  = gHeightmap.SampleLevel(heightmapSampler, topTex0,	 0).r;

	float3 tangent = normalize(float3(2.f, rightY - leftY, 0.f));
	float3 bitan   = normalize(float3(0.f, bottomY - topY, -2.f));
	float3 normalW = cross(tangent, bitan);

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


		float3 toEye = gCameraPosition - input.positionW;
		float distToEye = length(toEye); 
		toEye /= distToEye;
		float4 litColor = texColor;

		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		//[loop]
		for( uint i = 0;i < POINTLIGHTS; i++ )
		{
			float shadow;
			shadow = 1.0f;
			/*if ( gPointLights[i].Pad > 0 )
			{
				shadow = 0.0f;
				uint x = gPointLights[i].Pad*6;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] ); x++;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] ); x++;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] ); x++;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] ); x++;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] ); x++;
				shadow += ComputeShadowMap( mul( float4(input.positionW,1.f), gPLVP[x] ), gPLightShadow[x] );
			}*/

			float4 A,D,S;
			ComputePointLight(gMaterial, gPointLights[i], input.positionW, normalW, toEye, A, D, S);
			ambient += A;
			diffuse += shadow*D;
			spec += shadow*S;
		}
		[loop]
		for( uint i = 0;i < DIRECTIONALLIGHTS; i++ )
		{
			float shadow;
			if ( gDirectionalLights[i].Pad == 1.f )
				shadow = ComputeShadowMap( mul( float4(input.positionW,1.f), gDLVP0 ), gDLightShadow0 );
			else if ( gDirectionalLights[i].Pad == 2.f )
				shadow = ComputeShadowMap( mul( float4(input.positionW,1.f), gDLVP1 ), gDLightShadow1 );
			else
				shadow = 1.0f;

			float4 A,D,S;
			ComputeDirectionalLight(gMaterial, gDirectionalLights[i], normalW, toEye, A, D, S);
			ambient += A;
			diffuse += shadow*D;
			spec += shadow*S;
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
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(wireframeRS);
	}
}