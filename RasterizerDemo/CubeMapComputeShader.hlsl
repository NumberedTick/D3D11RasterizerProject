RWTexture2D<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t2);
Texture2D<float4> colorGBuffer : register(t1);
Texture2D<float4> normalGBuffer : register(t0);
Texture2D<float4> ambientGBuffer : register(t3);
Texture2D<float4> diffuesGBuffer : register(t4);
Texture2D<float4> specularGBuffer : register(t5);


cbuffer LightBuffer : register(b0)
{
    float lightIntensity;
    float3 lightPosition;
    float4 lightColor;
};

cbuffer cameraPosition : register(b1)
{
    float3 cameraPosition;
};

cbuffer ConstantBuffer2 : register(b2)
{
    row_major float4x4 viewProjection;
};

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 position = positionGBuffer[DTid.xy];
    float4 colour = colorGBuffer[DTid.xy];
    float3 normal = normalize(normalGBuffer[DTid.xy].xyz);
    
    float4 ambientRGBA = float4(ambientGBuffer[DTid.xy].xyz, 1.0f);
    float ambientIntensity = ambientGBuffer[DTid.xy].w;
    
    float4 diffuseRGBA = diffuesGBuffer[DTid.xy];
    
    float4 specularRGBA = float4(specularGBuffer[DTid.xy].xyz, 1.0f);
    float specularPower = specularGBuffer[DTid.xy].w;
    
    // Ambient Lighting Calculation
    float4 ambientFinal = ambientRGBA * ambientIntensity;
    
    // Diffuse Lighting Calculation
    float3 lightDirectionVector = lightPosition - position;
    float lightRadius = length(lightDirectionVector);
    float3 lightVector = (1 / lightRadius) * lightDirectionVector;
    float normalCheck = max(dot(normal, lightDirectionVector.xyz), 0);
    float quadIntensity = 1 / (pow(lightRadius, 2)) * lightIntensity;
    
    float4 diffuseFinal = lightColor * quadIntensity * normalCheck * diffuseRGBA;
    
    // Specular Highlight Calculation
    float3 cameraDirectionVector = cameraPosition - position;
    float3 cameraVector = (1 / length(cameraDirectionVector)) * cameraDirectionVector; // v
    float3 halfVector = (lightVector + cameraVector) / (length(lightVector + cameraVector)); // h
    float specularIntensity = pow(max(dot(normal, halfVector.xyz), 0), specularPower);

    float4 specularFinal = lightColor * specularRGBA * specularIntensity;
    
    // Combine lightning parts
    float4 finalColor = colour * ambientFinal + colour * diffuseFinal + colour * specularFinal;
    
    float4 debugColor = float4(viewProjection[0].xyz, 1);
    //float4 debugColor2 = float4(0, 0, -1, 1);
   
    backBufferUAV[DTid.xy] = finalColor;

}