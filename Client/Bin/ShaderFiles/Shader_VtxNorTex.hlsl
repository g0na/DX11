// 버텍스 셰이더
// 정점 셰이더는 정점을 가지고 논다 라는 말과 비슷하다.

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
// vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
// vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
// vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

Texture2D g_DiffuseTexture[2];
Texture2D g_MaskTexture;

vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};
   
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);      // 픽셀의 위치는 뷰포트이기 때문에 월드 공간으로 변환해서 넘겨준다.
    
    return Out;
}

// 아래 과정은 vPosition에만 적용하고 vTexCoord에는 적용하지 않는다.
// 1. 포지션 시멘틱에 해당하는 변수의 w로 모든 성분을 나눈다. 
// 2. 뷰포트로 변환한다. (윈도우 좌표로 변환)
// 3. 래스터라이즈

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{   
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // g_DiffuseTexture.Sample(어떤 방식으로 샘플링할지, 어디의 색을 얻어올지)
    
    vector vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexCoord * 50.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexCoord * 50.f);
    
    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexCoord);
    
    vector vMtrlDiffuse = vDestDiffuse * vMask.r + vSourDiffuse * (1.f - vMask.r);
    
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    
    float4 vLook = In.vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 50);
    
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
    (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
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