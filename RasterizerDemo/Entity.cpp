#include "Entity.h"
#include "MeshD3D11.h"
#include "ShaderResourceTextureD3D11.h"
#include "TextureD3D11.h"
#include "DirectXMath.h"
#include "ConstantBufferD3D11.h"
#include "PipelineHelper.h"



void Entity::Initialize(ID3D11Device* device, const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& roation, const DirectX::XMFLOAT3& scale,
	const MeshData& meshData, const TextureD3D11& textureObject, bool isCubeMap)
{
	/*
	* Function Flow
	* 
	* Assings the poition, rotation and scale of the entity
	* 
	* Generate a temporary world matrix
	* Turn the world matrix into a ConstantBufferD3D11 object
	* 
	* Initilize a mesh object from meshData
	* 
	* Save the given TextureD3D11 object from the paramiters (change current paramiters)
	* 
	* Generate a bounding box from the meshData from MeshD3D11. 
	* boundingBox.CreateFromPoints(boundingBox, meshData.VertexInfo.nrOfVerticesInBuffer, 
									meshData.VertexInfo.vertexData, sizeof(SimpleVertex));
	* 
	* Assing the cubeMap boolean to the given value
	
	*/

	// Assingning the values for the world matrix
	this->position = position;
	this->rotation = roation;
	this->scale = scale;

	// Generating the matrices that make up the world matrix
	DirectX::XMMATRIX transformationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// Creating the world matrix and initializing the world matrix buffer
	DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * transformationMatrix;
	DirectX::XMFLOAT4X4 worldMatrixFloat4x4;
	DirectX::XMStoreFloat4x4(&worldMatrixFloat4x4, worldMatrix);
	this->worldMatrixBuffer.Initialize(device, sizeof(DirectX::XMFLOAT4X4), &worldMatrixFloat4x4);

	// Initialize the mesh object
	//this->mesh.Initialize(device, meshData);

	// Assign the texture object
	this->texture = textureObject;
	
	// Create the bounding box from the meshData
	DirectX::BoundingBox entityBoundingBox;
	//entityBoundingBox.CreateFromPoints(entityBoundingBox, meshData.vertexInfo.nrOfVerticesInBuffer, meshData.vertexInfo.vertexData, sizeof(SimpleVertex));
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