#include "Entity.h"
#include "MeshD3D11.h"
#include "ShaderResourceTextureD3D11.h"

void Entity::Initialize(ID3D11Device* device, const MeshData& meshData,
	const std::string& modelName, const std::string& textureName,
	const std::string& pathToTextureFile, bool isCubeMap)
{
	// ..
}


bool Entity::isCubeMap() const
{
	return this->cubeMap;
}


DirectX::BoundingBox& Entity::setBoundingBox(DirectX::BoundingBox& boundingBox)
{
	this->boundngBox = boundingBox;
	return this->boundngBox;
}