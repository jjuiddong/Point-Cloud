
// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float4x4 mWVPT;
float3 vEyePos; // 카메라 위치.
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
    MipFilter = LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};


// ------------------------------------------------------------
// 그림자맵
// ------------------------------------------------------------
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

	AddressU = Clamp;
	AddressV = Clamp;
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
    MipFilter = LINEAR;

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
    MipFilter = LINEAR;
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
    MipFilter = LINEAR;
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
    MipFilter = LINEAR;
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
    MipFilter = LINEAR;
};


struct VS_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


// 모델 + 그림자.
struct VS_OUTPUT_SHADOW
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float4 TexShadow : TEXCOORD1;
	float3 Eye : TEXCOORD2;
	float3 N : TEXCOORD3;
};



// -------------------------------------------------------------
// 0패스: 지형 + 조명
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos : POSITION,          // 모델정점
	  float3 Normal : NORMAL,		// 법선벡터
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터

	float4 worldPos = mul(Pos, mWorld);
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}

// -------------------------------------------------------------
// 0패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);
    return Out;
}



// -------------------------------------------------------------
// 1패스: 지형 + 조명 + 포그
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos : POSITION,          // 모델정점
	  float3 Normal : NORMAL,		// 법선벡터
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	float4 worldPos = mul(Pos, mWorld);
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}

// -------------------------------------------------------------
// 1패스: 픽셀셰이더, 지형 + 조명 + 포그 출력.
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 2패스: 지형 + 조명 + 그림자.
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass2(
		float4 Pos : POSITION,          // 모델정점
		float3 Normal : NORMAL,		// 법선벡터
		float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;        // 출력데이터
    
	float4 worldPos = mul(Pos, mWorld);
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, mWVPT );
    
    return Out;
}


// -------------------------------------------------------------
// 2패스:픽셀셰이더,		지형 + 조명 + 포그 + 그림자
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = 	light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(1.3f - shadow);

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}


// -------------------------------------------------------------
// 3패스: 지형 + 조명
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass3(
      float4 Pos : POSITION,          // 모델정점
	  float3 Normal : NORMAL,		// 법선벡터
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;        // 출력데이터
    
	float4 worldPos = mul(Pos, mWorld);
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 3패스:픽셀셰이더,		지형 + 조명 + 포그 + 스플래팅 출력
// -------------------------------------------------------------
float4 PS_pass3(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = 	light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / alphaUVFactor));
	Out = (alpha.a * (color * tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (color * tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (color * tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (color * tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 4패스:픽셀셰이더,		지형 + 스플래팅  (조명X, 포그X)
// -------------------------------------------------------------
float4 PS_pass4(VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / alphaUVFactor));
	Out = (alpha.a * (tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

    return Out;
}



// -------------------------------------------------------------
// 5패스:픽셀셰이더,		지형 + 조명 + 포그 + 스플래팅 출력 + 그림자.
// -------------------------------------------------------------
float4 PS_pass5(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = 	light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / alphaUVFactor));
	Out = (alpha.a * (color * tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (color * tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (color * tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (color * tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(color - (0.8f*shadow));

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}


	
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{

	// 지형 + 조명
    pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }


	// 지형 + 조명 + 포그
    pass P1
    {
        VertexShader = compile vs_3_0 VS_pass1();
		PixelShader  = compile ps_3_0 PS_pass1();
    }


    // 지형 + 조명 + 그림자
    pass P2
    {
        VertexShader = compile vs_3_0 VS_pass2();
	PixelShader  = compile ps_3_0 PS_pass2();
    }


	// 지형 + 조명 + 포그 + 스플래팅
    pass P3
    {
        VertexShader = compile vs_3_0 VS_pass3();
		PixelShader  = compile ps_3_0 PS_pass3();
    }


	// 지형 + 스플래팅 (조명X, 포그X) (지형 텍스쳐 저장용)
    pass P4
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass4();
    }


	// 지형 + 조명 + 포그 + 스플래팅 + 그림자
    pass P5
    {
        VertexShader = compile vs_3_0 VS_pass2();
		PixelShader  = compile ps_3_0 PS_pass5();
    }
}
