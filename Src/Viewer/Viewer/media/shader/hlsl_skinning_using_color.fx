
// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float3 vLightDir = {0, -1, 0};
float3 vEyePos;

// 광원 밝기.
float4 I_a = {0.3f, 0.3f, 0.3f, 0.0f}; // ambient
float4 I_d = {1.f, 1.f, 1.f, 0.0f}; // diffuse
float4 I_s = {1.f, 1.f, 1.f, 0.0f}; // diffuse

// 반사율
float4 K_a = {1.0f, 1.0f, 1.0f, 1.0f}; // ambient 
float4 K_d = {1.0f, 1.0f, 1.0f, 1.0f}; // diffuse

// 팔레트
float4x3 mPalette[ 64] : SKINBONEMATRIX3;


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


// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : POSITION;
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
	float2 Tex : TEXCOORD0,		// 텍스쳐 좌표
	float4 Weights : BLENDWEIGHT,	// 버텍스 가중치
	float4 BoneIndices : BLENDINDICES// 본 인덱스 (4개 저장)
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0; // 출력데이터
    
	// 좌표변환
	float4x4 mWVP = mul(mWorld, mVP);

	// BoneIndices 는 0~1 사이 값으로 스케일링된 상태이기 때문에 
	// rgba *  255 를 해줘야 어플리케이션에서 저장한 BoneIndex를 
	// 얻어올 수 있다. D3DCOLORtoUBYTE4() 함수가 그 역할을 하게 된다.
	//
	// ARGB 형태로 어플리케이션 단에서 저장 한 값이 
	// D3DCOLORtoUBYTE4() 함수를 거치면서 BGRA 형태로 바뀐다.
	// 저장한 순서대로 Bone Index 를 접근하려면 wzyx 순으로 접근해야 한다.
	int4 IndexVector = D3DCOLORtoUBYTE4(BoneIndices);

	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, mPalette[ IndexVector.w]) * Weights.x;
	p += mul(Pos, mPalette[ IndexVector.z]) * Weights.y;
	p += mul(Pos, mPalette[ IndexVector.y]) * Weights.z;
	p += mul(Pos, mPalette[ IndexVector.x]) * Weights.w;

	n += mul(Normal, mPalette[ IndexVector.w]) * Weights.x;
	n += mul(Normal, mPalette[ IndexVector.z]) * Weights.y;
	n += mul(Normal, mPalette[ IndexVector.y]) * Weights.z;
	n += mul(Normal, mPalette[ IndexVector.x]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	n = normalize(n);

	// 법선 벡터 계산.
	float3 N = normalize( mul(n, (float3x3)mWIT) ); // 월드 좌표계에서의 법선.
	
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

	float3 L = -vLightDir.xyz;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	Out = 	I_a * K_a
				+ I_d * K_d * max(0, dot(N,L));
				+ I_s * pow( max(0, dot(N,H)), 16);

	Out = Out * tex2D(colorMap, In.Tex);

	//Out = tex2D(colorMap, In.Tex);
    return Out;
}

	
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }

}

