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

sampler ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = BACK;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = FRONT;
};

RasterizerState RS_Fog
{
    FillMode = Solid;
    CullMode = None;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Sky
{
    DepthEnable = false;
    DepthWriteMask = zero;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_Fog
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = LESS_EQUAL;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
};

BlendState BS_Fog
{
    BlendEnable[0] = true;

    SrcBlend = src_alpha;
    DestBlend = one;
    BlendOp = add;
};