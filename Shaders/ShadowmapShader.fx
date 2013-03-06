cbuffer cbPerObj
{
	matrix			gWVP;
}

struct vertexIn
{
	float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD;
};

// ------------------------
// --- VertexShader
// ------------------------
float4 VS(vertexIn input) : SV_POSITION
{
	return mul(float4(input.Pos, 1.0f), gWVP);
}
RasterizerState Depth
{
	// You need to experiment with these values for your scene.
	DepthBias = 1000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};
technique11 ShaderTech
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );

		SetRasterizerState(Depth);
	}
};