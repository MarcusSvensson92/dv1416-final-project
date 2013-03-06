#define POINTLIGHTS 10
#define DIRECTIONALLIGHTS 2

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

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

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
				   out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;	
	float d = length(lightVec);
	if( d > L.Range )
		return;
	lightVec /= d; 

	ambient = mat.Ambient * L.Ambient;	

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}

	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
	diffuse *= att;
	spec    *= att;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float Pad;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye,
						 out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -L.Direction;

	ambient += mat.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float specPower = max(mat.Specular.a,1.0f);

		float3 v = reflect(-lightVec, normal);

		float specFactor = pow(max(dot(v,toEye),0.0f), specPower);

		diffuse	+= diffuseFactor * mat.Diffuse * L.Diffuse;
		spec	+= specFactor * mat.Specular * L.Specular;
	}
};

cbuffer Lights
{
	PointLight			gPointLights[POINTLIGHTS];
	DirectionalLight	gDirectionalLights[DIRECTIONALLIGHTS];
};