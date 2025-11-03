// 버텍스 셰이더
// 정점 셰이더는 정점을 가지고 논다 라는 말과 비슷하다.

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};
   
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexCoord = In.vTexCoord;
    
    return Out;
}

// 아래 과정은 vPosition에만 적용하고 vTexCoord에는 적용하지 않는다.
// 1. 포지션 시멘틱에 해당하는 변수의 w로 모든 성분을 나눈다. 
// 2. 뷰포트로 변환한다. (윈도우 좌표로 변환)
// 3. 래스터라이즈

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // g_Texture.Sample(어떤 방식으로 샘플링할지, 어디의 색을 얻어올지)
    
    vector vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord);
    
    Out.vColor = vColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    // 한 technique에는 Default pass 이외에도 여러 pass가 들어올 수 있다.
}