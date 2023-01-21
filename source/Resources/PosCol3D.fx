float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;
float4x4 gInverseMatrix : ViewInverseMatrix;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

Texture2D <float4> gTexture;

//input/output structs
struct VS_INPUT
{
	float3 Position : POSITION;	
	float4 WorldPosition : COLOR;
	float2 uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : COLOR;
	float2 uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
	output.WorldPosition = mul(float4(input.Position, 1.0f), gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldViewProj);
	output.Normal = mul(normalize(input.Normal), (float3x3) gWorldViewProj);
	output.uv = input.uv;
	return output;
} 

float3 MaxToOne(float3 finalColor)
{
	float maxColor = float(max(finalColor.x, max(finalColor.y, finalColor.z)));

	if (maxColor > 1.f)
		return finalColor / maxColor;

	return finalColor;
}

float4 PixelShade(VS_OUTPUT input, SamplerState state)
{
	float lightIntensity = 7.0f;
	float shininess = 25.f;
	float pi = 3.14159f;
	float3 lightDirection = normalize(float3(0.577f, -0.577f, 0.577f));
	float3 ambient = float3(0.025f, 0.025f, 0.025f);

	float3 normal = float4(gNormalMap.Sample(state, input.uv)).xyz;
	float3 specular = float4(gSpecularMap.Sample(state, input.uv)).x;
	float glossiness = float4(gGlossinessMap.Sample(state, input.uv)).x * shininess;

	float3 binormal = cross(input.Normal, input.Tangent);
	float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);

	normal = 2 * normal - float3(1.f, 1.f, 1.f);
	normal = normalize(mul(normal, tangentSpaceAxis));

	float3 viewDirection = normalize(input.WorldPosition.xyz - gInverseMatrix[3].xyz);

	//observed area
	float observedArea = saturate(dot(normal, -lightDirection));

	//lambert
	float3 lambert = (observedArea * float4(gDiffuseMap.Sample(state, input.uv)) * lightIntensity) / pi;
	lambert = saturate(lambert);

	//phong
	float3 reflection = reflect(normal, -lightDirection);
	float cosine = saturate(max(0.f, dot(reflection, viewDirection)));
	float3 phong = saturate(specular * pow(cosine, glossiness));


	float3 finalColor = saturate(lambert * observedArea) + phong + ambient;
	finalColor = MaxToOne(finalColor);

	return float4(finalColor, 1.0f);
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
	return PixelShade(input, samPoint);
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
	return PixelShade(input, samLinear);
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
	return PixelShade(input, samAnisotropic);
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

