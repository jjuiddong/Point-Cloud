
// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float3 vEyePos;
float shininess = 32;
float4 globalAmbient = {0.2f, 0.2f, 0.2f, 1.0f};

// 팔레트
float4x3 mPalette[ 64];

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
};


// ------------------------------------------------------------
// 노멀맵
// ------------------------------------------------------------
texture normalMapTexture;
sampler2D normalMap = sampler_state
{
    Texture = <normalMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


// ------------------------------------------------------------
// 정반사맵
// ------------------------------------------------------------
texture specularMapTexture;
sampler2D specularMap = sampler_state
{
    Texture = <specularMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


// ------------------------------------------------------------
// Self Illumination 맵
// ------------------------------------------------------------
texture selfIllumMapTexture;
sampler2D selfIllumMap = sampler_state
{
    Texture = <selfIllumMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


struct VS_SHADOW_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
};


struct VS_BUMP_OUTPUT
{
    float4 Pos	 : POSITION;
	float2 Tex : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 Specular : COLOR1;
	float3 HalfVector : TEXCOORD1;
	float3 LightDir : TEXCOORD2;
};



// -------------------------------------------------------------
// 0패스: 스키닝 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
	float4 Pos : POSITION,          // 모델정점
	float3 Normal : NORMAL,		// 법선벡터
	float2 Tex : TEXCOORD0,		// 텍스쳐 좌표
	float4 Weights : TEXCOORD1,	// 버텍스 가중치
	float4 BoneIndices : TEXCOORD2 // 본 인덱스 (4개 저장)
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0; // 출력데이터
    
	// 좌표변환
	float4x4 mWVP = mul(mWorld, mVP);

	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]).xyz * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]).xyz * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]).xyz * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]).xyz * Weights.w;

	n += mul(float4(Normal,0), mPalette[ BoneIndices.x]).xyz * Weights.x;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.y]).xyz * Weights.y;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.z]).xyz * Weights.z;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.w]).xyz * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	n = normalize(n);

	// 법선 벡터 계산.
	float3 N = normalize( mul(n, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.
	
	Out.N = N;
	Out.Eye = vEyePos - mul(Pos, mWorld).xyz;
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

	float4 Out = 	light.ambient * material.ambient
						+ light.diffuse * material.diffuse * max(0, dot(N,L));
						+ light.specular * pow( max(0, dot(N,H)), shininess);

	Out = Out * tex2D(colorMap, In.Tex);
    return Out;
}


// -------------------------------------------------------------
// 2패스:정점셰이더, 그림자 맵 출력.
// -------------------------------------------------------------
VS_SHADOW_OUTPUT VS_pass1(
	float4 Pos : POSITION,          // 모델정점
	float3 Normal : NORMAL,		// 법선벡터
	float2 Tex : TEXCOORD0,		// 텍스쳐 좌표
	float4 Weights : TEXCOORD1,	// 버텍스 가중치
	float4 BoneIndices : TEXCOORD2 // 본 인덱스 (4개 저장)
)
{
	VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0; // 출력데이터
    
	// 좌표변환
	float4x4 mWVP = mul(mWorld, mVP);

	float3 p = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]) * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]) * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]) * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	Out.Diffuse = float4(1,1,1,1);
    
    return Out;
}



// -------------------------------------------------------------
// 4패스: 스키닝 + 범프 + 정반사 매핑 정점 셰이더
// D3D9NormalMapping 소스를 참조 함.
// http://www.dhpoware.com/demos/d3d9NormalMapping.html
// -------------------------------------------------------------
VS_BUMP_OUTPUT VS_pass4(
	float4 Pos : POSITION,          // 모델정점
	float3 Normal : NORMAL,		// 법선벡터
	float2 Tex : TEXCOORD0,		// 텍스쳐 좌표
	float4 Weights : TEXCOORD1,	// 버텍스 가중치
	float4 BoneIndices : TEXCOORD2, // 본 인덱스 (4개 저장)
	float3 tangent : TANGENT,		// tangent 벡터
	float3 binormal : BINORMAL 	// binormal 벡터
)
{
	VS_BUMP_OUTPUT Out = (VS_BUMP_OUTPUT)0; // 출력데이터
    
	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]).xyz * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]).xyz * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]).xyz * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]).xyz * Weights.w;

	n += mul(float4(Normal,0), mPalette[ BoneIndices.x]).xyz * Weights.x;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.y]).xyz * Weights.y;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.z]).xyz * Weights.z;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.w]).xyz * Weights.w;

	float3 worldPos = mul(float4(p,1), mWorld).xyz;
	float3 lightDir = -light.dir;
	float3 viewDir = vEyePos - worldPos;
	float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));

			  n = normalize( mul(n, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.
	float3 t = normalize( mul(tangent, (float3x3)mWIT) ); // 월드 좌표계에서의 탄젠트
	float3 b = normalize( mul(binormal, (float3x3)mWIT) ); // 월드 좌표계에서의 바이노멀
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);

	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Tex = Tex;
	Out.HalfVector = mul(halfVector, tbnMatrix);
	Out.LightDir = mul(lightDir, tbnMatrix);    
	Out.Diffuse = material.diffuse * light.diffuse;
	Out.Specular = material.specular * light.specular;

    return Out;
}


// -------------------------------------------------------------
// 4패스:픽셀셰이더, 스키닝 + 범프 + 정반사 매핑
// -------------------------------------------------------------
float4 PS_pass4(VS_BUMP_OUTPUT In) : COLOR
{
	// 스타2 노멀맵은 rgba 순서로 저장된게 아니라. bgxr 형태로 저장되어 있다.
	// 그래서 노멀값을 제대로 가져오려먼 agr 값을 가져와야 rgb즉 xyz 순서대로 
	// 가져오게 된다.
	// http://blog.naver.com/scahp/130109083917
	// http://forum.xentax.com/viewtopic.php?f=16&t=6119&start=15
	float3 n = normalize(tex2D(normalMap, In.Tex).agr * 2.0f - 1.0f);
    float3 h = normalize(In.HalfVector);
    float3 l = normalize(In.LightDir);

	float4 albedo = tex2D(colorMap, In.Tex);
	float4 specular = tex2D(specularMap, In.Tex);
	float4 emissive = tex2D(selfIllumMap, In.Tex);

    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, shininess);

    float3 color = (light.diffuse.rgb * albedo.rgb * nDotL) 
						+ light.specular.rgb * specular.rgb *  power;

	float3 ambient = light.ambient.rgb * emissive.rgb;

	return  float4(ambient + color, 1);
}


// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 기본 조명 스키닝 애니메이션
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }

    pass P1
    {
        // 그림자 맵 셰이더
        VertexShader = compile vs_3_0 VS_pass1();
    }

    pass P2
    {
	// 아무것도 없음.
    }

    pass P3
    {
	// 아무것도 없음.
    }

    pass P4
    {
		// 스키닝 애니메이션 + 범프 맵.
        VertexShader = compile vs_3_0 VS_pass4();
		PixelShader  = compile ps_3_0 PS_pass4();
    }

}

