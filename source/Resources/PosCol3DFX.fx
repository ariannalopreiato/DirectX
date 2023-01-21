float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;

//input/output structs
struct VS_INPUT
{
	float3 Position : SV_POSITION;
	float3 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = float4(input.Position, 1.0f);
	output.Color = input.Color;
	return output;
}

//pixel shader

//point
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 PSP(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color, 1.0f);
}

technique11 PointTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSP()));
	}
}


//linear 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 PSL(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color, 1.0f);
}

technique11 LinearTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSL()));
	}
}

//anisotropic
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 PSA(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color, 1.0f);
}

technique11 AnisotropicTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSA()));
	}
}

