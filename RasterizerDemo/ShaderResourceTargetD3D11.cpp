#include "ShaderResourceTextureD3D11.h"
#include "stb_image.h"
#include <iostream>




ShaderResourceTextureD3D11::ShaderResourceTextureD3D11(ID3D11Device* device, UINT width, UINT height, void* textureData)
{
	Initialize(device, width, height, textureData);
}
ShaderResourceTextureD3D11::ShaderResourceTextureD3D11(ID3D11Device* device, const char* pathToTextureFile) 
{
	Initialize(device, pathToTextureFile);
}
ShaderResourceTextureD3D11::~ShaderResourceTextureD3D11()
{
	if (this->srv)
	{
		this->srv->Release();
		this->srv = nullptr;
	}
	if (this->texture)
	{
		this->texture->Release();
		this->texture = nullptr;
	}
}

void ShaderResourceTextureD3D11::Initialize(ID3D11Device* device, UINT width, UINT height, void* textureData)
{
	// Check if the texture and srv are already initialized, if so release them
	if (this->texture)
	{
		this->texture->Release();
		this->texture = nullptr;
	}

	if (this->srv)
	{
		this->srv->Release();
		this->srv = nullptr;
	}


	// Create the texture description 
	DXGI_SAMPLE_DESC TextureSampleDesc;
	TextureSampleDesc.Count = 1;
	TextureSampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC Tex2DDesc;
	Tex2DDesc.Width = width;
	Tex2DDesc.Height = height;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.ArraySize = 1;
	Tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Tex2DDesc.SampleDesc = TextureSampleDesc;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Tex2DDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA Tex2DData;
	Tex2DData.pSysMem = textureData;
	Tex2DData.SysMemPitch = width * 4;
	Tex2DData.SysMemSlicePitch = 0;

	// Create the Texture2D
	HRESULT hr = device->CreateTexture2D(&Tex2DDesc, &Tex2DData, &this->texture);

	if (FAILED(hr))
	{
		std::cerr << "Error creating Texture2D!" << std::endl;
		return;
	}
	// Creates the SRV description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = Tex2DDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// Create the Shader Resource View
	hr = device->CreateShaderResourceView(this->texture, &srvDesc, &this->srv);
	if (FAILED(hr))
	{
		std::cerr << "Error creating Shader Resource View!" << std::endl;
		return;
	}
}

void ShaderResourceTextureD3D11::Initialize(ID3D11Device* device, const char* pathToTextureFile)
{
	// Check if the texture and srv are already initialized, if so release them
	if (this->texture)
	{
		this->texture->Release();
		this->texture = nullptr;
	}

	if (this->srv)
	{
		this->srv->Release();
		this->srv = nullptr;
	}
	
	// Load the image data using stb_image
	int textureWidth, textureHeight, numChannels;
	unsigned char* imageData = stbi_load(pathToTextureFile, &textureWidth, &textureHeight, &numChannels, 4);

	// Intilizes the texture with the loaded image data
	Initialize(device, textureWidth, textureHeight, imageData);
	stbi_image_free(imageData);

}

ID3D11ShaderResourceView* ShaderResourceTextureD3D11::GetSRV() const
{
	return this->srv;
}