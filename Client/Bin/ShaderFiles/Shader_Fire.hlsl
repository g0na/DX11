#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture_Noise, g_Texture_Color, g_Texture_Alpha;
float g_fTimeDelta;

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
    
    vector vColor = g_Texture_Color.Sample(ClampSampler, In.vTexCoord);
    vector vAlpha = g_Texture_Alpha.Sample(ClampSampler, In.vTexCoord);
    
    Out.vColor.rgb = vColor.rgb;
    Out.vColor.a = vAlpha.r;
    
    return Out;

}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Fog);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}