float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;

Texture2D <float4> gTexture;

//input/output structs
struct VS_INPUT
{
	float3 Position : POSITION;	
	float3 Color : COLOR;
	float2 uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR; 
	float2 uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//output.Position = float4(input.Position, 1.0f);
	output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
	output.Color = input.Color;
	output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldViewProj);
	output.Normal = mul(normalize(input.Normal), (float3x3) gWorldViewProj);
	output.uv = input.uv;
	return output;
}

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

//pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//Sample(gDiffuseMap);
	input.Color = gDiffuseMap.Sample(samPoint, input.uv);
	return float4(input.Color, 1.f);
}

technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

