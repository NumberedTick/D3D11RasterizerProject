#include "PipelineHelper.h"
#include <DirectXMath.h>
#include "MeshD3D11.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "OBJ_Loader.h"


#include <fstream>
#include <string>
#include <iostream>

using namespace DirectX;

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11ComputeShader*& cShader ,std::string& vShaderByteCode)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("VertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();
	reader.open("PixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	reader.open("ComputeShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open CS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &cShader)))
	{
		std::cerr << "Failed to create compute shader!" << std::endl;
		return false;
	}

	return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UVCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

// Function to create a world matrix with a new angle
// Used mostly for the rotation
XMMATRIX CreateWorldMatrix(float angle, float xDist)
{
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.0+xDist);
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
	XMMATRIX worldMatrix = XMMatrixMultiply(translationMatrix, rotationMatrix);
	return worldMatrix;
}

// Function for creating a view+perspective matrix in world space
XMMATRIX CreatViewPerspectiveMatrix()
{
	XMVECTOR focusPoint = { 0.0f, 0.0f, 1.0f };
	XMVECTOR upDirection = { 0.0f, 1.0f, 0.0f };
	XMVECTOR eyePosition = { 0.0f, 0.0f, -4.0f };
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection); 
	XMMATRIX perspectiveFovMatrix = XMMatrixPerspectiveFovLH(XM_PI / 2.5f, 1024.0f / 576.0f, 0.1f, 1000.0f);
	XMMATRIX viewAndPerspectiveMatrix = XMMatrixMultiply(viewMatrix, perspectiveFovMatrix);

	return viewAndPerspectiveMatrix;
}

bool LoadVertexs(std::string& modleName, std::vector<SimpleVertex>& modelVertexes) 
{
	objl::Loader objLoader;
	bool objFileCheck = objLoader.LoadFile(modleName);

	if (!objFileCheck)
	{
		std::cerr << "Error loading OBJ file! File name not found!" << std::endl;
		return false;
	}

	for (int j = 0; j < objLoader.LoadedMeshes.size(); ++j)
	{
		for (int k = 0; k < objLoader.LoadedMeshes[j].Vertices.size(); ++k) {
			SimpleVertex Vertex;
			Vertex.pos[0] = objLoader.LoadedVertices[k].Position.X;
			Vertex.pos[1] = objLoader.LoadedVertices[k].Position.Y;
			Vertex.pos[2] = objLoader.LoadedVertices[k].Position.Z;

			Vertex.norm[0] = -objLoader.LoadedVertices[k].Normal.X;
			Vertex.norm[1] = -objLoader.LoadedVertices[k].Normal.Y;
			Vertex.norm[2] = -objLoader.LoadedVertices[k].Normal.Z;

			Vertex.UV[0] = objLoader.LoadedVertices[k].TextureCoordinate.X;
			Vertex.UV[1] = objLoader.LoadedVertices[k].TextureCoordinate.Y;

			modelVertexes.push_back(Vertex);
		}
	}
	return true;
}

bool LoadIndices(std::string& modleName, std::vector<unsigned int>& indices)
{
	objl::Loader objLoader;
	bool objFileCheck = objLoader.LoadFile(modleName);

	if (!objFileCheck)
	{
		std::cerr << "Error loading OBJ file! File name not found!" << std::endl;
		return false;
	}


	// Loads indices into a vector
	for (int i = 0; i < objLoader.LoadedIndices.size(); ++i)
	{
		indices.push_back(objLoader.LoadedIndices[i]);

	}

	// Swaps every second and thrid element in the vector due to the OBJ parder being made for OpenGLs right handed rendering
	/*
	for (int i = 0; i < objLoader.LoadedIndices.size() / 3; ++i)
	{
		int temp = indices[3 * i + 1];
		indices[3 * i + 1] = indices[3 * i + 2];
		indices[3 * i + 2] = temp;
	}
	*/


}


bool CreateConstantBufferVertex(ID3D11Device* device, ID3D11Buffer*& constantBufferVertex)
{
	// Creation of the world matrix and the Veiw + perspecive matrix
	XMMATRIX worldMatrix = CreateWorldMatrix(0.0f, 0.0f);
	XMMATRIX viewAndPerspectiveMatrix = CreatViewPerspectiveMatrix();

	// Adding the two matrixes into one array
	XMFLOAT4X4 float4x4Array[2];
	MatrixBuffer matrixBuffer(worldMatrix, viewAndPerspectiveMatrix, float4x4Array);

	// Buffer Desc for constant buffer
	D3D11_BUFFER_DESC constantBufferDesc; 
	constantBufferDesc.ByteWidth = sizeof(XMFLOAT4X4)*2;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &matrixBuffer;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantBufferDesc, &constantData, &constantBufferVertex);

	return !FAILED(hr);
}

// Creation of the Vertex Buffer
bool CreateVertexBuffer(ID3D11Device* device, VertexBufferD3D11**& testVertexBuffer, std::vector<std::string>& modelNames)
{
	
	for (int i = 0; i < modelNames.size(); i++) 
	{
		std::vector<SimpleVertex> Vertices;



		if (!LoadVertexs(modelNames[i], Vertices))
		{
			return false;
		}


		testVertexBuffer[i]->Initialize(device, sizeof(SimpleVertex), Vertices.size(), Vertices.data());
		// Buffer des for vertex Buffer

		if (testVertexBuffer[i]->GetBuffer() == nullptr)
			return false;
	}

	return true;
}

bool CreateIndexBuffer(ID3D11Device* device, IndexBufferD3D11**& testIndexBuffer, std::vector<std::string>& modelNames)
{
	std::vector<unsigned int> indices;

	for (int i = 0; i < modelNames.size(); i++)
	{
		if (!LoadIndices(modelNames[i], indices))
		{
			return false;
		}

		testIndexBuffer[i]->Initialize(device, indices.size(), indices.data());

		if (testIndexBuffer[i]->GetBuffer() == nullptr)
		{
			return false;
		}
	}
	

	return true;
}


bool Create2DTexture(ID3D11Device* device, ID3D11Texture2D*& texture, std::string textureName) 
{
	// Loading the texture

	int textureWidth, textureHeight, numChannels;
	unsigned char* imageData = stbi_load(textureName.c_str(), &textureWidth, &textureHeight, &numChannels, 4);
	
	// Creating nesicary sampler for the texture 2d desc
	DXGI_SAMPLE_DESC TextureSampleDesc;
	TextureSampleDesc.Count = 1;
	TextureSampleDesc.Quality = 0;

	// texture 2D description
	D3D11_TEXTURE2D_DESC Tex2DDesc;
	Tex2DDesc.Width = textureWidth;
	Tex2DDesc.Height = textureHeight;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.ArraySize = 1;
	Tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Tex2DDesc.SampleDesc = TextureSampleDesc;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	Tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Tex2DDesc.MiscFlags = 0;

	// Data for the texture 2D
	D3D11_SUBRESOURCE_DATA Tex2DData;
	Tex2DData.pSysMem = imageData;
	Tex2DData.SysMemPitch = textureWidth*(numChannels+1);
	Tex2DData.SysMemSlicePitch = 0;

	// Creation of the Texture2D
	HRESULT hr = device->CreateTexture2D(&Tex2DDesc, &Tex2DData, &texture);
	return !FAILED(hr);
}

bool CreateGBuffer(ID3D11Device* device, ID3D11Texture2D*& gBuffer, ID3D11RenderTargetView*& gBufferRtv,ID3D11ShaderResourceView*& gBufferSrv, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC textDesc;
	textDesc.Width = width;
	textDesc.Height = height;
	textDesc.MipLevels = 1;
	textDesc.ArraySize = 1;
	textDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;

	gBuffer = nullptr;
	gBufferRtv = nullptr;
	gBufferSrv = nullptr;

	HRESULT hr = device->CreateTexture2D(&textDesc, nullptr, &gBuffer);

	if (FAILED(hr))
	{
		std::cerr << "Error Creating G-Buffer Texture" << std::endl;
		return false;
	}

	hr = device->CreateRenderTargetView(gBuffer, nullptr, &gBufferRtv);

	if (FAILED(hr))
	{
		std::cerr << "Error Creating G-Buffer RTV" << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(gBuffer, nullptr, &gBufferSrv);

	if (FAILED(hr))
	{
		std::cerr << "Error Creating G-Buffer SRV" << std::endl;
		return false;
	}

	return !FAILED(hr);
	
}



bool CreateSRV(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv) 
{
	// Creation of the SRV
	HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &srv);
	return !FAILED(hr);
}

bool CreateSampler(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
	// deffinition of the texture sampler description
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_EQUAL;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 1;

	// Creation of the texture sampler
	HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	return !FAILED(hr);
}

// Function for crating a constant buffer for a light to be sent to the pixel shader
bool CreateLightBuffer(ID3D11Device* device, ID3D11Buffer*& constantLightBuffer)
{
	// Deffining the pramiters for the light in view space
	std::array<float, 4> lightPosition = { 512.0f, 288.0f, 175.0f, 0.0f};
	std::array<float, 4> lightColor = { 1.0f, 1.0f, 1.0f, 1.0f};
	float lightIntencity = 50.0f;

	// Creation of a point light
	PointLight pointLight = { lightPosition,  lightColor, lightIntencity};

	// Descritption of a constant buffer for light
	D3D11_BUFFER_DESC constantBufferLightDesc; 
	constantBufferLightDesc.ByteWidth = 48;
	constantBufferLightDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferLightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferLightDesc.MiscFlags = 0;
	constantBufferLightDesc.StructureByteStride = 0;

	// Data for the constant light buffer
	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &pointLight;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantBufferLightDesc, &constantData, &constantLightBuffer);

	return !FAILED(hr);
}

// Function for Creating a constant buffer for a material to be sent to the Pixel shader
bool CreateMaterialBuffer(ID3D11Device* device, ID3D11Buffer*& constantMaterialBuffer) 
{
	// Defining paramiters of the material
	// Color values
	std::array<float, 4> ambientColor = { 1.0f ,1.0f ,1.0f ,1.0f };
	std::array<float, 4> diffuseColor = { 1.0f,1.0f,1.0f,1.0f };
	std::array<float, 4> specularColor = { 1.0f,1.0f,1.0f,1.0f };
	
	// Material Coeficients
	float ambientIntensity = 0.2f;
	float padding = 0.0f; 
	float specularPower =  100.0f;
	
	// Creation of the material
	Material material = { ambientColor, diffuseColor, specularColor, ambientIntensity, padding, specularPower };

	// Descritption of a constant buffer for light
	D3D11_BUFFER_DESC constantBufferMaterialDesc;
	constantBufferMaterialDesc.ByteWidth = sizeof(Material);
	constantBufferMaterialDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferMaterialDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferMaterialDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferMaterialDesc.MiscFlags = 0;
	constantBufferMaterialDesc.StructureByteStride = 0;

	// Data for the constant light buffer
	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &material;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantBufferMaterialDesc, &constantData, &constantMaterialBuffer);
	return !FAILED(hr);
}

// Function to create a constant buffer for the camera position for speculatr highlights
bool CreateCameraBuffer(ID3D11Device* device, ID3D11Buffer*& constantCameraBuffer)
{
	// Creation of an array that can be uploaded to the Pixel Shader in view space
	std::array<float, 4> cameraPosition = { 512, 288, 496.5f, 0.0f };

	D3D11_BUFFER_DESC constantCameraBufferDesc;
	constantCameraBufferDesc.ByteWidth = sizeof(cameraPosition);
	constantCameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantCameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantCameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantCameraBufferDesc.MiscFlags = 0;
	constantCameraBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &cameraPosition;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantCameraBufferDesc, &constantData, &constantCameraBuffer);
	return !FAILED(hr);

}

bool SetupPipeline(ID3D11Device* device, VertexBufferD3D11**& vertexBuffer, IndexBufferD3D11**& indexBuffer,  ID3D11VertexShader*& vShader,
	ID3D11PixelShader*& pShader, ID3D11ComputeShader*& cShader ,ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBufferVertex, 
	ID3D11Buffer*& constantLightBuffer, ID3D11Buffer*& constantMaterialBuffer, ID3D11Buffer*& constantCameraBuffer, 
	ID3D11DeviceContext*& deviceContext, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& sampleState, std::vector<std::string>& modelNames)
{
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, cShader,vShaderByteCode))
	{
		std::cerr << "Error loading shaders!" << std::endl;
		return false;
	}

	if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}


	if (!CreateConstantBufferVertex(device, constantBufferVertex))
	{
		std::cerr << "Error creating constant buffer for Vertex shader!" << std::endl;
		return false;
	}


	if (!CreateVertexBuffer(device, vertexBuffer, modelNames))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}
	
	if (!CreateIndexBuffer(device, indexBuffer, modelNames))
	{
		std::cerr << "Error creating index buffer!" << std::endl;
		return false;
	}
	/*
	if (!Create2DTexture(device, texture))
	{
		std::cerr << "Error creating 2D Texture" << std::endl;
		return false;
	}
	*/
	
	/*
	if (!CreateSRV(device, texture, srv))
	{
		std::cerr << "Error creating Shader Resorse View!" << std::endl;
		return false;
	}
	*/
	

	if (!CreateSampler(device, sampleState))
	{
		std::cerr << "Error creating Sampler!" << std::endl;
		return false;
	}

	if (!CreateLightBuffer(device, constantLightBuffer))
	{
		std::cerr << "Error creating Constant Buffer for light!" << std::endl;
		return false;
	}

	if (!CreateMaterialBuffer(device, constantMaterialBuffer))
	{
		std::cerr << "Error creating Constant Buffer for material!" << std::endl;
		return false;
	}

	if (!CreateCameraBuffer(device, constantCameraBuffer))
	{
		std::cerr << "Error creating Constant Buffer for Camera position!" << std::endl;
		return false;
	}
	// Binding the necessary Buffers and Resources to the diffrent shaders

	deviceContext->VSSetConstantBuffers(0, 1, &constantBufferVertex);
	//deviceContext->PSSetShaderResources(0, 1, &srv);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	ID3D11Buffer* bufferArray[3] = { constantLightBuffer, constantMaterialBuffer, constantCameraBuffer };
	deviceContext->CSSetConstantBuffers(0, 3, bufferArray);

	//deviceContext->PSSetConstantBuffers(3, 1, &constantCameraBuffer);

	return true;
}