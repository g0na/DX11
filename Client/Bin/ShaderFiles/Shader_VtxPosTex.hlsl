#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
float g_fHpRatio, g_fStaminaRatio;

/*삼각형리스트로 그린다 -> 인덱스버퍼에 있는 인덱스 세개에 해당하는 정점 세개롤 뽑아온다. */ 
/* 각각 vs_main에 집어넣어서 함수 세개를 각각 동시에 호출하여 정점 쉐이더를 시작한다. */

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

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 
   
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
// 3. 래스터라이즈 : 반환된 정점 정보를 기반하여 사이를 선형보간한 픽셀을 만들어준다.

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
    if (vColor.a <= 0.3f)
        discard;
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_HP(PS_IN In)
{
    PS_OUT Out;
    
    // g_Texture.Sample(어떤 방식으로 샘플링할지, 어디의 색을 얻어올지)
    
    vector vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord);
    if (vColor.a <= 0.5f)
        discard;
    
    if (In.vTexCoord.x > g_fHpRatio)
        discard;
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_STAMINA_GAUGE(PS_IN In)
{
    PS_OUT Out;
    
    // g_Texture.Sample(어떤 방식으로 샘플링할지, 어디의 색을 얻어올지)
    
    vector vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord);
    if (vColor.a <= 0.2f)
        discard;
    
    if (In.vTexCoord.x > 0.62f)
        discard;
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_STAMINA(PS_IN In)
{
    PS_OUT Out;
    
    vector vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord);
    //if (vColor.a <= 0.2f)
    //    discard;
    
    if (In.vTexCoord.x > g_fStaminaRatio)
        discard;
    
    Out.vColor = vColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_HP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HP();
    }

    pass UI_Stamina_Gauge
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STAMINA_GAUGE();
    }

    pass UI_Stamina
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STAMINA();
    }

    // 한 technique에는 Default pass 이외에도 여러 pass가 들어올 수 있다.
}