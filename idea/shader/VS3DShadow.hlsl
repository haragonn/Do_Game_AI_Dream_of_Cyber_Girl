cbuffer ConstantBuffer : register(b1)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float4 color;
    float4 light;
    float4x4 matLightView;  // ライトビュー変換行列
    float4x4 matLightProj;  // 射影変換行列
}

// マテリアルバッファ
struct MATERIAL
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emission;
    float shininess;
};

//cbuffer MaterialBuffer : register(b4)
//{
//    MATERIAL	Material;
//}


struct VS_IN
{
    float4 pos : POSITION0;
    float4 nor : NORMAL0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION; // 射影変換座標
    float2 tex : TEXCOORD0;
    float4 ZCalcTex : TEXCOORD1; // Z値算出用テクスチャ
    float4 col : COLOR0; // ディフューズ色
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;

    float4x4 w = world;
    float3 nor;
    float  col;
    float4x4 wvp;

    wvp = mul(world, view);
    wvp = mul(wvp, projection);
    output.pos = mul(input.pos, wvp);

    w._41 = 0.0f;
    w._42 = 0.0f;
    w._43 = 0.0f;

    nor = mul(normalize(input.nor), w).xyz;
    nor = normalize(nor);

    col = saturate(-dot(normalize(light.xyz), nor));
    col = (col * 0.3f + 0.7f);

    output.col = float4(col, col, col, 1.0f) * color;
    output.tex = input.tex;

    // ライトの目線によるワールドビュー射影変換をする
    wvp = mul(world, matLightView);
    wvp = mul(wvp, matLightProj);
    output.ZCalcTex = mul(input.pos, wvp);

    return output;
}