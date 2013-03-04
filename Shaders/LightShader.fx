Texture2D		gDiffuseMap;

// ------------------------
// --- Input and Output Structures
// ------------------------
struct PointLight
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float Pad;
};

cbuffer cbPerFrame
{
	matrix gWorld;
	matrix gWorldViewProj;
	float3 gCameraPosition;

	PointLight gLight;
};

cbuffer cbConstants
{
	float2 gTexCoords[4] = { float2(0.f, 1.f),
							 float2(0.f, 0.f),
							 float2(1.f, 1.f),
							 float2(1.f, 0.f) };
};

SamplerState linSampler
{
	Filter	 = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

BlendState AlphaBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

struct GSIn
{
	float3 positionW : POSITION;
	float2 sizeW	 : SIZE;
};

struct PSIn
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW	 : NORMAL_W;
	float2 tex0		 : TEX0;
};

GSIn VS()
{
	GSIn output;
	float size = 2.f;
	output.positionW = mul(float4(gLight.Position, 1.f), gWorld).xyz;
	output.sizeW	 = float2(size,size);
	return output;
}

[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> stream)
{
	static const float PI = 3.14159265f;

	float3 up = float3(0.f, 1.f, 0.f);
	float3 look;

	look = gCameraPosition - input[0].positionW;
	look = normalize(look);
	float3 right = cross(up, look);
	right = normalize(right);
	up = cross(look, right);
	float r = -right.x;
				
	float halfWidth  = 0.5f * input[0].sizeW.x;
	float halfHeight = 0.5f * input[0].sizeW.y;

	float4 positions[4];
	positions[0] = float4(input[0].positionW + (halfWidth * +right + halfHeight * -up), 1.f);
	positions[1] = float4(input[0].positionW + (halfWidth * +right + halfHeight * +up), 1.f);
	positions[2] = float4(input[0].positionW + (halfWidth * -right + halfHeight * -up), 1.f);
	positions[3] = float4(input[0].positionW + (halfWidth * -right + halfHeight * +up), 1.f);

	PSIn output;
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		output.tex0		 = gTexCoords[i];
		output.positionH = mul(positions[i], gWorldViewProj);
		output.positionW = positions[i].xyz;
		output.normalW	 = look;

		stream.Append(output);
	}
}

float4 PS(PSIn input) : SV_TARGET
{
	float4 texColor = gDiffuseMap.Sample( linSampler, input.tex0 );
	float4 litColor = texColor*gLight.Diffuse;
	return litColor;
}

technique11 RenderTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(CompileShader(gs_4_0, GS()));
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(NULL);

		SetBlendState(AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
	}
}