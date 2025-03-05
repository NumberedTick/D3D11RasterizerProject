#pragma once

#include "MeshD3D11.h"
#include "ShaderResourceTextureD3D11.h"

#include <d3d11_4.h>

class Entity
{
private:
	// Save IDs instead of lage data
	// use Maps
	// create meshID 
	// MeshD3D11 mesh;
	std::string modelName;
	std::string textureName;
	ShaderResourceTextureD3D11 textureSRV;
	ID3D11Buffer* materialBuffer = nullptr;
	bool cubeMap = false;

public:
	Entity() = default;
	~Entity() = default;
	Entity(const Entity& other) = delete;
	Entity& operator=(const Entity& other) = delete;
	Entity(Entity&& other) = delete;
	Entity& operator=(Entity&& other) = delete;

	void Initialize(ID3D11Device* device, const MeshData& meshData, 
		const std::string& modelName, const std::string& textureName, 
		const std::string& pathToTextureFile, bool isCubeMap);

	MeshD3D11 getMesh() const;
	std::string getModelName() const;
	std::string getTextureName() const;
	ShaderResourceTextureD3D11 getTextureSRV() const;
	ID3D11Buffer* getMaterialBuffer() const;
	bool isCubeMap() const;

	void performDrawCall(ID3D11DeviceContext* context) const;
};