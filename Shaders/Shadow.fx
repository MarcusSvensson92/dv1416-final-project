Texture2D	gDLightShadow0: register(t0);
Texture2D	gDLightShadow1: register(t1);

int sMapSize;

SamplerState shadSampler
{   
	Filter = MIN_MAG_MIP_POINT;

	AddressU = BORDER;
	AddressV = BORDER;
};
SamplerComparisonState shadSampler2
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;

    ComparisonFunc = LESS;
};

float ComputeShadowMap(float4 lpos, Texture2D sMap)
{
	lpos.xyz /= lpos.w;
	
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if( lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z < 0.0f  || lpos.z > 1.0f ) 
		return 0.0f;

    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x/2 + 0.5;
    lpos.y = lpos.y/-2 + 0.5;

	float dx = 1.0f/sMapSize;

	float percentLit = 0.0f;
	const float2 offsets[9] = 
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};
	[unroll]
	for(int i = 0; i < 9; ++i)
	{
		percentLit += sMap.SampleCmpLevelZero(shadSampler2, 
			lpos.xy + offsets[i], lpos.z).r;
	}

	return percentLit / 9.0f;
};

cbuffer Shadows
{
	matrix		gPLVP[POINTLIGHTS * 6];
	matrix		gDLVP0;
	matrix		gDLVP1;
};