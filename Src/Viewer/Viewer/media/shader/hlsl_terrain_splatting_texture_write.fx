
// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float4x4 mWVPT;
float3 vEyePos;
float4 vFog;
float4 fogColor = {0.58823f, 0.58823f, 0.58823f, 1}; // RGB(150,150,150)
float alphaUVFactor = 8.f;


struct Light
{
	float3 dir;				// world space direction
	float3 pos;				// world space position
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float spotInnerCone;	// spot light inner cone (theta) angle
	float spotOuterCone;	// spot light outer cone (phi) angle
	float radius;           // applies to point and spot lights only
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 specular;
	float shininess;
};

Light light;
Material material;



// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture colorMapTexture;
sampler colorMap = sampler_state
{
    Texture = <colorMapTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};


// ------------------------------------------------------------
// 스플래팅 알파 텍스쳐
// ------------------------------------------------------------
texture SplattingAlphaMap;
sampler SplattingMapSamp = sampler_state
{
    Texture = <SplattingAlphaMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};


// ------------------------------------------------------------
// 스플래팅 텍스처 레이어1
// ------------------------------------------------------------
texture Tex1;
sampler Samp1 = sampler_state
{
    Texture = <Tex1>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
};

// ------------------------------------------------------------
// 스플래팅 텍스처 레이어2
// ------------------------------------------------------------
texture Tex2;
sampler Samp2 = sampler_state
{
    Texture = <Tex2>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
};

// ------------------------------------------------------------
// 스플래팅 텍스처 레이어3
// ------------------------------------------------------------
texture Tex3;
sampler Samp3 = sampler_state
{
    Texture = <Tex3>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
};


// ------------------------------------------------------------
// 스플래팅 텍스처 레이어4
// ------------------------------------------------------------
texture Tex4;
sampler Samp4 = sampler_state
{
    Texture = <Tex4>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
};



// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


// -------------------------------------------------------------
// 1패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos : POSITION,          // 모델정점
	  float3 Normal : NORMAL,		// 법선벡터
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 좌표변환
	float4x4 mWVP = mul(mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );

	// 법선 벡터 계산.
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.

	Out.N = N;
	Out.Eye = vEyePos - Pos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 1패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float4 Out;
	Out = tex2D(colorMap, In.Tex);
    return Out;
}


// -------------------------------------------------------------
// 2패스:픽셀셰이더, 스플래팅  (조명X)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float4 Out;

	Out = tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / alphaUVFactor));
	Out = (alpha.a * (tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

    return Out;
}


	
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
	// 기본 텍스쳐 출력 (스플래팅 X)
	pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }


	// 스플래팅 
    pass P1
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass1();
    }

}
