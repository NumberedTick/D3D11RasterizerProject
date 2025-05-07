#pragma once

#include <d3d11_4.h>

#include <string>

class TextureD3D11
{
private:
	std::string textureName;
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	//ID3D11SamplerState* samplerState = nullptr;
};