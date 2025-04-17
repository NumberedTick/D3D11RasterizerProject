#include "MeshD3D11.h"

// Initialization function
void MeshD3D11::Initialize(ID3D11Device* device, const MeshData& meshInfo)
{
	VertexBufferD3D11 meshVertexBuffer;
	IndexBufferD3D11 meshIndexBuffer;
	SubMeshD3D11 subMesh;

	meshVertexBuffer.Initialize(device, meshInfo.vertexInfo.nrOfVerticesInBuffer, meshInfo.vertexInfo.sizeOfVertex, meshInfo.vertexInfo.vertexData);
	meshIndexBuffer.Initialize(device, meshInfo.indexInfo.nrOfIndicesInBuffer, meshInfo.indexInfo.indexData);
	subMesh.Initialize(meshInfo.subMeshInfo[0].startIndexValue, meshInfo.subMeshInfo[0].nrOfIndicesInSubMesh, meshInfo.subMeshInfo[0].ambientTextureSRV, meshInfo.subMeshInfo[0].diffuseTextureSRV, meshInfo.subMeshInfo[0].specularTextureSRV);
}

void MeshD3D11::BindMeshBuffers(ID3D11DeviceContext* context) const
{

}

void MeshD3D11::PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const
{

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