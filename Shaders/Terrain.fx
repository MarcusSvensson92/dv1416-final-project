#include		"Light.fx"
#include		"Shadow.fx"

cbuffer cbPerFrame
{
	matrix gWorld;
	matrix gWorldViewProj;
	float3 gCameraPosition;

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
			ComputePointLight(gMaterial, gPointLights[i], input.positionW, input.normalW, toEye, A, D, S);
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
			ComputeDirectionalLight(gMaterial, gDirectionalLights[i], input.normalW, toEye, A, D, S);
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
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(wireframeRS);
	}
}