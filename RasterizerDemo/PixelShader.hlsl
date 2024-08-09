Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uvcoords : UVCOORDS;
};

struct PixelShaderOutput
{
    float4 position : SV_Target2;
    float4 colour : SV_Target1;
    float4 normal : SV_Target0;
    float4 ambient : SV_Target3;
    float4 diffuse : SV_Target4;
    float4 specular : SV_Target5;
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
	
    output.colour = textureMap.Sample(samplerState, input.uvcoords);
    output.normal = float4(input.normal, 0);
    output.position = input.position;
    output.ambient = float4(ambientRGBA.xyz, ambientIntensity);
    output.diffuse = diffuseRGBA;
    output.specular = float4(specularRGBA.xyz, specularPower);
	
        return output;
    };

//Temp disable output
/*
float4 main(PixelShaderInput input) : SV_TARGET
{
	

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
*/