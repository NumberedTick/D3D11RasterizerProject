struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPos : WORLDS_POS;
    float3 normal : NORMAL;
    float2 uvcoords : UVCOORDS;
};


TextureCube reflectionTexture : register(t0);

sampler standardSampler : register(s0);

cbuffer CameraInfo : register(b1)
{
    float3 cameraPos;
};

struct PixelShaderOutput
{
    float4 position : SV_Target2;
    float4 colour : SV_Target1;
    float4 normal : SV_Target0;
    float4 ambient : SV_Target3;
    float4 diffuse : SV_Target4;
    float4 specular : SV_Target5;
    float4 worldPos : SV_Target6;
};



cbuffer MaterialBuffer : register(b0)
{
    float4 ambientRGBA;
    float4 diffuseRGBA;
    float4 specularRGBA;
    float ambientIntensity;
    float specularPower;
    float padding;

};


PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
	
    float3 normal = normalize(input.normal);
    float3 incomingView = normalize(input.worldPos.xyz-cameraPos);
    float3 reflectedView = reflect(incomingView, normal);
    float4 sampledValue = reflectionTexture.Sample(standardSampler, reflectedView);
    
    //output.colour = sampledValue;
    //output.colour = reflectionTexture.Sample(standardSampler, float3(input.uvcoords, 5));
    output.normal = float4(input.normal, 0);
    output.colour = float4(1, 0, 0, 1);
    output.position = float4(input.worldPos, 0);
    output.ambient = float4(ambientRGBA.xyz, 1);
    output.diffuse = diffuseRGBA;
    output.specular = float4(specularRGBA.xyz, specularPower);
	
    return output;
};