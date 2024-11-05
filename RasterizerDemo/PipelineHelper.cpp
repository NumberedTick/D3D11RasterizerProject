#include "PipelineHelper.h"
#include <DirectXMath.h>
#include "MeshD3D11.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "CameraD3D11.h"
#include "D3D11Helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "OBJ_Loader.h"


#include <fstream>
#include <string>
#include <iostream>

using namespace DirectX;

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11PixelShader*& pShaderCubeMap, ID3D11ComputeShader*& cShader ,std::string& vShaderByteCode)
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

	reader.open("CubeMapPixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open Cube Map PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShaderCubeMap)))
	{
		std::cerr << "Failed to create Cube Map pixel shader!" << std::endl;
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
XMMATRIX CreateWorldMatrix(float angle, float xDist, float yDist, float zDist)
{
	XMMATRIX translationMatrix = XMMatrixTranslation(xDist, yDist, zDist);
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
	XMMATRIX worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);


	return worldMatrix;
}

// Function for creating a view+perspective matrix in world space
XMMATRIX CreatViewPerspectiveMatrix(XMVECTOR viewVector, XMVECTOR upDirection, XMVECTOR eyePosition, float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	XMMATRIX viewMatrix = XMMatrixLookToLH(eyePosition, viewVector, upDirection); 
	XMMATRIX perspectiveFovMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
	XMMATRIX viewAndPerspectiveMatrix = XMMatrixMultiply(viewMatrix, perspectiveFovMatrix);

	return viewAndPerspectiveMatrix;
}

bool LoadObj(std::string& modleName, objl::Loader& objLoader)
{
	bool objFileCheck = objLoader.LoadFile(modleName);

	if (!objFileCheck)
	{
		std::cerr << "Error loading OBJ file! File name not found!" << std::endl;
		return false;
	}
}

bool LoadVertexs(std::string& modleName, std::vector<SimpleVertex>& modelVertexes) 
{
	objl::Loader objLoader;
	if (!LoadObj(modleName, objLoader))
	{
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
	if (!LoadObj(modleName, objLoader))
	{
		return false;
	}

	// Loads indices into a vector
	for (int i = 0; i < objLoader.LoadedIndices.size(); ++i)
	{
		indices.push_back(objLoader.LoadedIndices[i]);

	}


}
bool CreateWorldMatrixBuffer(ID3D11Device* device, ID3D11Buffer*& constantWorldMatrixBuffer)
{
	XMMATRIX worldMatrix = CreateWorldMatrix(0.0f, 0.0f, 0.0f, -0.5f);
	XMFLOAT4X4 worldMatrixFloat4X4;

	XMStoreFloat4x4(&worldMatrixFloat4X4, worldMatrix);

	// Buffer Desc for constant buffer
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &worldMatrixFloat4X4;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantBufferDesc, &constantData, &constantWorldMatrixBuffer);

	return !FAILED(hr);
}

bool CreateViewProjMatrixBuffer(ID3D11Device* device, ID3D11Buffer*& constantViewProjMatrixBuffer)
{
	// Creation of the world matrix and the Veiw + perspecive matrix
	XMVECTOR eyePosition = { 0.0f, 0.0f, -3.5f };
	XMVECTOR viewVecotr = { 0.0f, 0.0f, 1.0f };
	XMVECTOR upDirection = { 0.0f, 1.0f, 0.0f };
	float fovAgnleY = XM_PI / 2.5f;
	float aspectRatio = 1024.0f / 576.0f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	XMMATRIX viewAndPerspectiveMatrix = CreatViewPerspectiveMatrix(viewVecotr, upDirection, eyePosition,fovAgnleY, aspectRatio, nearZ,farZ);

	// Adding the two matrixes into one array
	XMFLOAT4X4 float4x4Array;
	XMStoreFloat4x4(&float4x4Array, viewAndPerspectiveMatrix);

	// Buffer Desc for constant buffer
	D3D11_BUFFER_DESC constantBufferDesc; 
	constantBufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &float4x4Array;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&constantBufferDesc, &constantData, &constantViewProjMatrixBuffer);

	return !FAILED(hr);
}

// Function for Creating a constant buffer for a material to be sent to the Pixel shader
bool CreateMaterialBuffer(ID3D11Device* device, ConstantBufferD3D11*& constantBuffer, Material& material)
{
	constantBuffer->Initialize(device, sizeof(Material), &material);
	return true;
}

bool CreateMaps(ID3D11Device* device, Material& material, std::string& modleName, ConstantBufferD3D11*& constantBuffer)
{
	objl::Loader objLoader;
	if (!LoadObj(modleName, objLoader))
	{
		return false;
	}

	// Color values
	std::array<float, 4> ambientColor = { objLoader.LoadedMaterials[0].Ka.X, objLoader.LoadedMaterials[0].Ka.Y, objLoader.LoadedMaterials[0].Ka.Z, 1.0f };
	std::array<float, 4> diffuseColor = { objLoader.LoadedMaterials[0].Kd.X,objLoader.LoadedMaterials[0].Kd.Y,objLoader.LoadedMaterials[0].Kd.Z,1.0f };
	std::array<float, 4> specularColor = { objLoader.LoadedMaterials[0].Ks.X,objLoader.LoadedMaterials[0].Ks.Y,objLoader.LoadedMaterials[0].Ks.Z,1.0f };

	// Material Coeficients
	float ambientIntensity = 0.0f;
	float padding = 0.0f;
	float specularPower = objLoader.LoadedMaterials[0].Ns;

	// Creation of the material
	material = { ambientColor, diffuseColor, specularColor, ambientIntensity, padding, specularPower };

	if (!CreateMaterialBuffer(device, constantBuffer, material))
	{
		return false;
	}
	return true;
}



// Creation of the Vertex Buffer
bool CreateVertexBuffer(ID3D11Device* device, VertexBufferD3D11**& testVertexBuffer, std::vector<std::string>& modelNames, Material**& materialArray, ConstantBufferD3D11**& materialBufferArray)
{
	
	for (int i = 0; i < modelNames.size(); i++) 
	{
		std::vector<SimpleVertex> Vertices;



		if (!LoadVertexs(modelNames[i], Vertices))
		{
			return false;
		}

		if (!CreateMaps(device, *materialArray[i], modelNames[i], materialBufferArray[i]))
		{
			return false;
		}

		testVertexBuffer[i]->Initialize(device, sizeof(SimpleVertex), Vertices.size(), Vertices.data());
		// Buffer des for vertex Buffer

		if (testVertexBuffer[i]->GetBuffer() == nullptr)
		{
			return false;
		}
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
	std::array<float, 3> lightPosition = { 1.0f, 0.0f, -3.5f };
	std::array<float, 4> lightColor = { 1.0f, 1.0f, 1.0f, 1.0f};
	float lightIntencity = 1.0f;

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



// Function to create a constant buffer for the camera position for speculatr highlights
bool CreateCameraBuffer(ID3D11Device* device, ID3D11Buffer*& constantCameraBuffer)
{
	// Creation of an array that can be uploaded to the Pixel Shader in view space
	std::array<float, 4> cameraPosition = { 0.0f, 0.0f, -3.5f };

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

// Function to create the Resources that only need to be created once for each texture cube
bool CreateTextrueCubeReusableResources(ID3D11Device* device, CameraD3D11**& cameraArray, D3D11_VIEWPORT& cubeMapViewport, ID3D11Texture2D*& dsTexture,
	ID3D11DepthStencilView*& dsView, ID3D11DepthStencilState*& dsState, ID3D11UnorderedAccessView*& uavTextureCube)
{

	UINT cubeWidth = 1024;
	UINT cubeHeight = 1024;

	ProjectionInfo projectionInfo;
	projectionInfo.fovAngleY = XM_PIDIV2;
	projectionInfo.aspectRatio = 1.0f;
	projectionInfo.nearZ = 0.1f;
	projectionInfo.farZ = 100.0f;
	float upRotations[6] = { XM_PIDIV2, -XM_PIDIV2, 0.0f, 0.0f, 0.0f, XM_PI };
	float rightRotations[6] = { 0.0f, 0.0f, -XM_PIDIV2 , XM_PIDIV2 , 0.0f, 0.0f };

	XMFLOAT3 initialPosition = { 0.0f, 0.0f, -0.5f };

	for (int i = 0; i < 6; ++i)
	{
		cameraArray[i]->Initialize(device, projectionInfo, initialPosition);
		cameraArray[i]->RotateUp(upRotations[i]);
		cameraArray[i]->RotateRight(rightRotations[i]);
	}
	// setting up the viewport for the cubeMap
	SetViewport(cubeMapViewport, cubeWidth, cubeHeight);

	// Creation of the depth stencil and depth stencil state for the cube map

	if (!CreateDepthStencil(device, cubeWidth, cubeHeight, dsTexture, dsView))
	{
		std::cerr << "Error creating depth stencil view!" << std::endl;
		return false;
	}

	if (!CreateDepthStencilState(device, dsState))
	{
		std::cerr << "Error creating depth stencil state!" << std::endl;
		return false;
	}

	ID3D11Texture2D* cubeMapBackBuffer;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = cubeWidth;
	desc.Height = cubeHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &cubeMapBackBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Error creating texture cube UAV Texture!" << std::endl;
		return false;
	}


	hr = device->CreateUnorderedAccessView(cubeMapBackBuffer, NULL, &uavTextureCube);
	if (FAILED(hr))
	{
		std::cerr << "Error creating texture cube UAV!" << std::endl;
		return false;
	}


	cubeMapBackBuffer->Release();
	return true;

}

// Function To create the resouces that are needed for each texture cube
bool CreateTextureCube(ID3D11Device* device, ID3D11Texture2D*& cubeMapTexture, ID3D11UnorderedAccessView**& cubeMapUavArray, ID3D11ShaderResourceView*& cubeMapSRV)
{
	UINT cubeWidth = 1024;
	UINT cubeHeight = 1024;

	//bool hasSRV = false;
	D3D11_TEXTURE2D_DESC desc; 
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = cubeWidth;
	desc.Height = cubeHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &cubeMapTexture);
	if (FAILED(hr))
	{
		return !FAILED(hr);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = 1;
	uavDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		uavDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateUnorderedAccessView(cubeMapTexture, &uavDesc, &cubeMapUavArray[i]);

		if (FAILED(hr))
		{
			return !FAILED(hr);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = 1;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(cubeMapTexture, nullptr, &cubeMapSRV);

	if (FAILED(hr))
	{
		return !FAILED(hr);
	}

	return true;
}

enum TEXTURE_CUBE_FACE_INDEX
{
	POSITIVE_X = 0,
	NEGATIVE_X = 1,
	POSITIVE_Y = 2,
	NEGATIVE_Y = 3,
	POSITIVE_Z = 4,
	NEGATIVE_Z = 5
};


bool SetupPipeline(ID3D11Device* device, VertexBufferD3D11**& vertexBuffer, IndexBufferD3D11**& indexBuffer,  ID3D11VertexShader*& vShader,
	ID3D11PixelShader*& pShader, ID3D11PixelShader*& pShaderCubeMap, ID3D11ComputeShader*& cShader ,ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantWorldMatrixBuffer, ID3D11Buffer*& constantViewProjMatrixBuffer,
	ID3D11Buffer*& constantLightBuffer, ID3D11Buffer*& constantMaterialBuffer, ID3D11Buffer*& constantCameraBuffer, 
	ID3D11DeviceContext*& deviceContext, ID3D11Texture2D*& cubeMapTexture, ID3D11UnorderedAccessView**& cubeMapUavArray,ID3D11ShaderResourceView*& cubeMapSrv, 
	CameraD3D11**& cameraArray, D3D11_VIEWPORT& cubeMapViewport, ID3D11Texture2D*& cubeMapDSTexture, ID3D11DepthStencilView*& cubeMapDSView, ID3D11DepthStencilState*& cubeMapDSState,
	ID3D11SamplerState*& sampleState, std::vector<std::string>& modelNames, UINT width, UINT height, Material**& materialArray, ConstantBufferD3D11**& materialBufferArray, 
	ID3D11UnorderedAccessView*& uavTextureCube)
{
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, pShaderCubeMap,cShader,vShaderByteCode))
	{
		std::cerr << "Error loading shaders!" << std::endl;
		return false;
	}

	if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	if (!CreateWorldMatrixBuffer(device, constantWorldMatrixBuffer))
	{
		std::cerr << "Error creating constant buffer for world matrix in Vertex shader!" << std::endl;
		return false;

	}

	if (!CreateViewProjMatrixBuffer(device, constantViewProjMatrixBuffer))
	{
		std::cerr << "Error creating constant buffer for View and Projection Matrix in Vertex shader!" << std::endl;
		return false;
	}


	if (!CreateVertexBuffer(device, vertexBuffer, modelNames, materialArray, materialBufferArray))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}
	
	if (!CreateIndexBuffer(device, indexBuffer, modelNames))
	{
		std::cerr << "Error creating index buffer!" << std::endl;
		return false;
	}
	

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

	if (!CreateCameraBuffer(device, constantCameraBuffer))
	{
		std::cerr << "Error creating Constant Buffer for Camera position!" << std::endl;
		return false;
	}	
	
	if (!CreateTextrueCubeReusableResources(device, cameraArray, cubeMapViewport, cubeMapDSTexture, cubeMapDSView, cubeMapDSState, uavTextureCube))
	{
		std::cerr << "Error creating Reusable TextureCube Resources!" << std::endl;
		return false;
	}

	if (!CreateTextureCube(device, cubeMapTexture, cubeMapUavArray, cubeMapSrv))
	{
		std::cerr << "Error creating G-Buffer for Texture Cube!" << std::endl;
		return false;
	}
	

	// Binding the necessary Buffers and Resources to the diffrent shaders

	
	//deviceContext->PSSetShaderResources(0, 1, &srv);
	deviceContext->PSSetSamplers(0, 1, &sampleState);


	ID3D11Buffer* bufferArray[2] = {constantLightBuffer, constantCameraBuffer};
	deviceContext->CSSetConstantBuffers(0, 2, bufferArray);


	return true;
}