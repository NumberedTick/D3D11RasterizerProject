struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uvcoords : UVCOORDS;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uvcoords : UVCOORDS;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 world;            
    float4x4 viewProjection;   
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    // Transform vertex position
    float4 vertexPosition = mul(float4(input.position, 1.0f), world);
    vertexPosition = mul(vertexPosition, viewProjection);
    output.position = vertexPosition;

    // Transform vertex normal
    float3 vertexNormal = mul(input.normal, (float3x3)world);
    output.normal = normalize(mul(vertexNormal, (float3x3)viewProjection));

    // Pass UV coordinates
    output.uvcoords = input.uvcoords;

    return output;
}
