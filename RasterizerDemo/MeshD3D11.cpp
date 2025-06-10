#include "MeshD3D11.h"

// Initialization function
void MeshD3D11::Initialize(ID3D11Device* device, const MeshData& meshInfo)
{
	//SubMeshD3D11 subMesh;
	this->meshData = meshInfo;
	this->vertexBuffer.Initialize(device, this->meshData.vertexInfo.sizeOfVertex, this->meshData.vertexInfo.nrOfVerticesInBuffer, this->meshData.vertexInfo.vertexData);
	this->indexBuffer.Initialize(device, this->meshData.indexInfo.nrOfIndicesInBuffer, this->meshData.indexInfo.indexData);
	//subMesh.Initialize(meshInfo.subMeshInfo[0].startIndexValue, meshInfo.subMeshInfo[0].nrOfIndicesInSubMesh, meshInfo.subMeshInfo[0].ambientTextureSRV, meshInfo.subMeshInfo[0].diffuseTextureSRV, meshInfo.subMeshInfo[0].specularTextureSRV);
}

void MeshD3D11::BindMeshBuffers(ID3D11DeviceContext* context) const
{

}

void MeshD3D11::PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const
{

}

void MeshD3D11::SetMaterialBuffer(ID3D11Buffer* materialBuffer)
{
	this->materialBuffer = materialBuffer;
}

size_t MeshD3D11::GetNrOfSubMeshes() const
{
	return this->subMeshes.size();
}

ID3D11ShaderResourceView* MeshD3D11::GetAmbientSRV(size_t subMeshIndex) const
{
	return this->subMeshes[subMeshIndex].GetAmbientSRV();
}

ID3D11ShaderResourceView* MeshD3D11::GetDiffuseSRV(size_t subMeshIndex) const
{
	return this->subMeshes[subMeshIndex].GetDiffuseSRV();
}

ID3D11ShaderResourceView* MeshD3D11::GetSpecularSRV(size_t subMeshIndex) const
{
	return this->subMeshes[subMeshIndex].GetSpecularSRV();
}

std::string MeshD3D11::GetMeshName() const
{
	return this->meshData.modelName;
}

UINT MeshD3D11::GetNrOfVertices() const
{
	return this->vertexBuffer.GetNrOfVertices();
}

UINT MeshD3D11::GetVertexSize() const
{
	return this->vertexBuffer.GetVertexSize();
}

UINT MeshD3D11::GetNrOfIndices() const
{
	return this->indexBuffer.GetNrOfIndices();
}

ID3D11Buffer* MeshD3D11::GetVertexBuffer() const
{
	return this->vertexBuffer.GetBuffer();
}

ID3D11Buffer* MeshD3D11::GetIndexBuffer() const
{
	return this->indexBuffer.GetBuffer();
}

ID3D11Buffer* MeshD3D11::GetMaterialBuffer() const
{
	return this->materialBuffer;
}