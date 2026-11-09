cbuffer cbTransformData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

cbuffer cbDirectionalLight : register(b1)
{
    float3 direction;
    float shininess;
    float4 color;
    float3 cameraPos;
    float specularStrength;
    float3 ambientColor;
    float intensity;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

cbuffer cbMaterial : register(b2)
{
    float4 materialDiffuse;
    float4 materialSpecular;
    float materialShininess;
    float3 materialPadding;
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
    float4 worldPosition : TEXCOORD1;
};

VertexOut VS(VertexIn input)
{
    VertexOut output;
    output.position = mul(float4(input.position, 1), world);
    output.worldPosition = output.position;
    output.position = mul(output.position, view);
    output.position = mul(output.position, proj);
    output.normal = mul(input.normal, (float3x3)world);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

float4 PS(VertexOut input) : SV_TARGET
{
    float3 ambient = ambientColor;
    float3 norm = normalize(input.normal);
    float3 light = normalize(-direction);
    float diffuseIntensity = saturate(dot(norm, light));
    float3 diffuse = color.rgb * diffuseIntensity * intensity;
    
    float3 vertical = normalize(cameraPos - input.worldPosition.xyz);
    float3 horizontal = normalize(light + vertical);
    float specularIntensity = pow(saturate(dot(norm, horizontal)), max(materialShininess, 1.0f));
    float3 specular = materialSpecular.rgb * color.rgb * specularIntensity * specularStrength * intensity;
    
    float4 albedo = input.color * materialDiffuse * diffuseTexture.Sample(diffuseSampler, input.uv);
    float4 outputCol = float4(albedo.rgb * (ambient + diffuse) + specular, albedo.a);
    return outputCol;
}
