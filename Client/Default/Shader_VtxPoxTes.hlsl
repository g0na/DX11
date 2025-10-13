// 버텍스 셰이더
// 정점 셰이더는 정점을 가지고 논다 라는 말과 비슷하다.

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
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

technique DefaultTechnique
{
    pass Default
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    // 한 technique에는 Default pass 이외에도 여러 pass가 들어올 수 있다.
}