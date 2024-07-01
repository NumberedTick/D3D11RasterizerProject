Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uvcoords : UVCOORDS;
};

cbuffer LightBuffer : register(b1)
{
	float4 lightPosition;
	float4 lightColor;
	float lightIntensity;
};

cbuffer MaterialBuffer : register(b2)
{
	float4 ambientRGBA;
	float4 diffuseRGBA;
	float4 specularRGBA;
	float ambientIntensity;
	float padding;
	float specularPower;
};

cbuffer cameraPosition : register(b3)
{
	float4 cameraPosition;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	// Texture sampling
	float4 texColor = textureMap.Sample(samplerState, input.uvcoords);

	// Ambient Lighting Calculation
	float4 ambientFinal = ambientRGBA * ambientIntensity;

	// Diffuse Lighting Calculation
	float4 lightDirectionVector = lightPosition - input.position; 
	float lightRadius = length(lightDirectionVector);
	float4 lightVector = (1 / lightRadius) * lightDirectionVector; 
	float normalCheck = max(dot(input.normal, lightDirectionVector.xyz), 0);
	float quadIntensity = 1/(pow(lightRadius,2))* lightIntensity; 

	float4 diffuseFinal = lightColor * quadIntensity * normalCheck * diffuseRGBA;

	// Specular Highlight Calculation
    float4 cameraDirectionVector = cameraPosition - input.position;
    float4 cameraVector = (1 / length(cameraDirectionVector)) * cameraDirectionVector; // v
    float4 halfVector = (lightVector + cameraVector) / (length(lightVector + cameraVector)); // h
    float specularIntensity = pow(max(dot(input.normal, halfVector.xyz), 0), specularPower);

	float4 specularFinal = lightColor * specularRGBA * specularIntensity;

	// Combine lightning parts
	float4 finalColor = texColor * ambientFinal + texColor * diffuseFinal + specularFinal;

	return finalColor;
}