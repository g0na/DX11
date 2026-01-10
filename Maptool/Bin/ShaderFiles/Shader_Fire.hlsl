#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture_Noise, g_Texture_Color, g_Texture_Alpha;
float g_fFrameTime, g_fDistortionScale, g_fDistortionBias;
float2 g_vDistortion1, g_vDistortion2, g_vDistortion3;
float3 g_vScrollSpeed, g_vScale;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vTexCoord1 : TEXCOORD1;
    float2 vTexCoord2 : TEXCOORD2;
    float2 vTexCoord3 : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float3 vRight = float3(g_ViewMatrix._11, 0.f, g_ViewMatrix._31);
    vRight = normalize(vRight);
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vLook = cross(vRight, vUp);
    
    float fScaleX = length(g_WorldMatrix._11_12_13);
    float fScaleY = length(g_WorldMatrix._21_22_23);
    
    float3 vObjectPos = float3(g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43);
    float3 vWorldPos = vObjectPos + (In.vPosition.x * fScaleX * vRight) + (In.vPosition.y * fScaleY * vUp);
    
    vector vPosition = vector(vWorldPos, 1.f);
    
    //vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexCoord = In.vTexCoord;
    
    // 3개 노이즈가 각각 다른 속도로 위로 올라가도록 하기
    // Compute texture coordinates for first noise texture using the first scale and upward scrolling speed values.
    Out.vTexCoord1 = (In.vTexCoord * g_vScale.x);
    Out.vTexCoord1.y = Out.vTexCoord1.y + (g_fFrameTime * g_vScrollSpeed.x);

    // Compute texture coordinates for second noise texture using the second scale and upward scrolling speed values.
    Out.vTexCoord2 = (In.vTexCoord * g_vScale.y);
    Out.vTexCoord2.y = Out.vTexCoord2.y + (g_fFrameTime * g_vScrollSpeed.y);

    // Compute texture coordinates for third noise texture using the third scale and upward scrolling speed values.
    Out.vTexCoord3 = (In.vTexCoord * g_vScale.z);
    Out.vTexCoord3.y = Out.vTexCoord3.y + (g_fFrameTime * g_vScrollSpeed.z);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vTexCoord1 : TEXCOORD1;
    float2 vTexCoord2 : TEXCOORD2;
    float2 vTexCoord3 : TEXCOORD3;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // g_Texture.Sample(어떤 방식으로 샘플링할지, 어디의 색을 얻어올지)

    
    vector vNoise1 = g_Texture_Noise.Sample(DefaultSampler, In.vTexCoord1);
    vector vNoise2 = g_Texture_Noise.Sample(DefaultSampler, In.vTexCoord2);
    vector vNoise3 = g_Texture_Noise.Sample(DefaultSampler, In.vTexCoord3);
    
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;
    
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

    vector vFinalNoise = vNoise1 + vNoise2 + vNoise3;
    
    // Perturb the input texture Y coordinates by the distortion scale and bias values.  
    // The perturbation gets stronger as you move up the texture which creates the flame flickering at the top effect.
    float fPerturb = ((1.0f - In.vTexCoord.y) * g_fDistortionScale) + g_fDistortionBias;

    // Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
    float2 vNoiseCoords;
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexCoord.xy;
    
    vector vColor = g_Texture_Color.Sample(ClampSampler, vNoiseCoords.xy);
    vector vAlpha = g_Texture_Alpha.Sample(ClampSampler, vNoiseCoords.xy);
    
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