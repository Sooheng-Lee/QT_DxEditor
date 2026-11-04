cbuffer cbTransformData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

VertexOut VS(VertexIn input)
{
    VertexOut output;
    output.position = mul(float4(input.position, 1), world);
    output.normal = input.normal;
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

float4 PS(VertexOut input) : SV_TARGET
{
    return input.color;
}