#include "Entity.h"
#include "DirectXMath.h"



void Entity::Initialize(ID3D11Device* device, const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& roation, const DirectX::XMFLOAT3& scale, const std::string& meshName, 
	const std::map<std::string, UINT>& meshIDMap, bool isCubeMap)
{
	/*
	* Function Flow
	* 
	* Assings the poition, rotation and scale of the entity
	* 
	* Generate a temporary world matrix
	* Turn the world matrix into a ConstantBufferD3D11 object
	* 
	* Take in the name of a mesh and assign it the meshID from the meshID map
	* Used to call upon the id of the mesh in the meshvector instead of storing the entire mesh inside of the entity to save memory
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

	DirectX::XMFLOAT4X4 worldMatrixFloat4x4 = this->SetWorldMatrix(); // Generate the world matrix and store it in a XMFLOAT4X4 object
	
	this->worldMatrixBuffer = ConstantBufferD3D11(device, sizeof(DirectX::XMFLOAT4X4), &worldMatrixFloat4x4);

	// Initialize the mesh object
	this->meshID = meshIDMap.at(meshName); // Sets meshID to the ID of the mesh with the given name

	// Assign the texture object
	
	// Create the bounding box from the meshData
	//entityBoundingBox.CreateFromPoints(entityBoundingBox, meshData.vertexInfo.nrOfVerticesInBuffer, meshData.vertexInfo.vertexData, sizeof(SimpleVertex));

	this->cubeMap = isCubeMap;
	this->initialized = true; // Set the initialized boolean to true. Used to be able to create more entites but not initilize them or try to render them
}


ID3D11Buffer* Entity::getWorldMatrixBuffer() const
{
	return this->worldMatrixBuffer.GetBuffer();
}

bool Entity::isCubeMap() const
{
	return this->cubeMap;
}

bool Entity::isInitialized() const
{
	return this->initialized;
}

UINT Entity::getMeshID() const
{
	return this->meshID;
}

void Entity::SetPosition(const DirectX::XMFLOAT3& pos)
{
	this->position = pos;
}

void Entity::SetRotation(const DirectX::XMFLOAT3& rot)
{
	this->rotation = rot;
}

void Entity::SetScale(const DirectX::XMFLOAT3& scale)
{
	this->scale = scale;
}

void Entity::UpdateInternalConstantBuffer(ID3D11DeviceContext* context)
{
	DirectX::XMFLOAT4X4 data = this->SetWorldMatrix();
	this->worldMatrixBuffer.UpdateBuffer(context, &data);
}

DirectX::XMFLOAT4X4 Entity::SetWorldMatrix()
{
	// Generating the matrices that make up the world matrix
	DirectX::XMMATRIX transformationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// Creating the world matrix and initializing the world matrix buffer
	DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * transformationMatrix;
	DirectX::XMFLOAT4X4 worldMatrixFloat4x4;
	DirectX::XMStoreFloat4x4(&worldMatrixFloat4x4, worldMatrix);
	return worldMatrixFloat4x4;
}

