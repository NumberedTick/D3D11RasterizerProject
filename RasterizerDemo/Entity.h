#pragma once

#include "MeshD3D11.h"
#include "ShaderResourceTextureD3D11.h"
#include "TextureD3D11.h"
#include "ConstantBufferD3D11.h"

#include <d3d11_4.h>
#include <DirectXCollision.h>

class Entity
{
private:

	DirectX::XMFLOAT3 position = { 0.0, 0.0, 0.0 };
	DirectX::XMFLOAT3 rotation = { 0.0, 0.0, 0.0 };
	DirectX::XMFLOAT3 scale = { 1.0, 1.0, 1.0 };

	// save world matrix
	ConstantBufferD3D11 worldMatrixBuffer; // for world matrix


	
	
	
	// use Maps
	// create meshID 
	MeshD3D11 mesh;
	TextureD3D11 texture;
	// Create Texture.h file for texture infor and buffers
	//std::string textureName;
	//ShaderResourceTextureD3D11 textureSRV;
	//ID3D11Buffer* materialBuffer = nullptr; maybe use in meshd3d11.h/.cpp
	DirectX::BoundingBox boundngBox;

	bool cubeMap = false;

public:
	Entity() = default;
	~Entity() = default;
	Entity(const Entity& other) = delete;
	Entity& operator=(const Entity& other) = delete;
	Entity(Entity&& other) = delete;
	Entity& operator=(Entity&& other) = delete;

	void Initialize(ID3D11Device* device, const DirectX::XMFLOAT3& position, 
		const DirectX::XMFLOAT3& roation, const DirectX::XMFLOAT3& scale, 
		const MeshData& meshData, const TextureD3D11& textureObject, 
		bool isCubeMap);

	//MeshD3D11 getMesh() const;
	std::string getModelName() const;
	std::string getTextureName() const;
	//ShaderResourceTextureD3D11 getTextureSRV() const;
	ID3D11Buffer* getMaterialBuffer() const;
	bool isCubeMap() const;
	int getModelID() const;
	int getTextureID() const;
	DirectX::BoundingBox& getBoundingBox() const;

	int setModelID(int id);
	int setTextureID(int id);
	DirectX::BoundingBox& setBoundingBox(DirectX::BoundingBox& boundingBox);

	//void performDrawCall(ID3D11DeviceContext* context) const;
};