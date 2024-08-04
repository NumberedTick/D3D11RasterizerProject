RWTexture2D<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t2);
Texture2D<float4> colorGBuffer : register(t1);
Texture2D<float4> normalGBuffer: register(t0);

cbuffer LightBuffer : register(b0)
{
    float4 lightPosition;
    float4 lightColor;
    float lightIntensity;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 ambientRGBA;
    float4 diffuseRGBA;
    float4 specularRGBA;
    float ambientIntensity;
    float padding;
    float specularPower;
};

cbuffer cameraPosition : register(b2)
{
    float4 cameraPosition;
};

[numthreads(8, 8, 1)]
void main( uint3 DTid:SV_DispatchThreadID)
{
    float4 position = positionGBuffer[DTid.xy];
    float4 colour = colorGBuffer[DTid.xy];
    float3 normal = normalize(normalGBuffer[DTid.xy].xyz);
    
    // Ambient Lighting Calculation
    float4 ambientFinal = ambientRGBA * ambientIntensity;
    
    // Diffuse Lighting Calculation
    float4 lightDirectionVector = lightPosition - position;
    float lightRadius = length(lightDirectionVector);
    float4 lightVector = (1 / lightRadius) * lightDirectionVector;
    float normalCheck = max(dot(normal, lightDirectionVector.xyz), 0);
    float quadIntensity = 1 / (pow(lightRadius, 2)) * lightIntensity;
    
    float4 diffuseFinal = lightColor * quadIntensity * normalCheck * diffuseRGBA;
    
    // Specular Highlight Calculation
    float4 cameraDirectionVector = cameraPosition - position;
    float4 cameraVector = (1 / length(cameraDirectionVector)) * cameraDirectionVector; // v
    float4 halfVector = (lightVector + cameraVector) / (length(lightVector + cameraVector)); // h
    float specularIntensity = pow(max(dot(normal, halfVector.xyz), 0), specularPower);

    float4 specularFinal = lightColor * specularRGBA * specularIntensity;
    
    // Combine lightning parts
    float4 finalColor = colour * ambientFinal+colour*diffuseFinal+colour*specularFinal;
   
    backBufferUAV[DTid.xy] = finalColor;

}