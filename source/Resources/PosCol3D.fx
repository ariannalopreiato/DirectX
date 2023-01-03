float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;

//input/output structs
struct VS_INPUT
{
	float3 Position : POSITION;	
	float3 Color : COLOR;
	float3 uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR; 
	float3 uv : TEXCOORD;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//output.Position = float4(input.Position, 1.0f);
	output.Position = mul(gWorldViewProj, float4(input.Position, 1.0f));
	output.Color = input.Color;
	output.uv = input.uv;
	return output;
}

//pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.uv, 1.f);
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

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; 
	AddressV = Wrap;
};