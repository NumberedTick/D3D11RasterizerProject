#pragma once

#include <vector>
#include <array>

#include <d3d11_4.h>

#include "SubMeshD3D11.h"
//#include "PipelineHelper.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "ConstantBufferD3D11.h"
#include <string>

struct MeshData
{
	std::string modelName; 

	struct VertexInfo
	{
		size_t sizeOfVertex;
		size_t nrOfVerticesInBuffer;
		void* vertexData;
	} vertexInfo;

	struct IndexInfo
	{
		size_t nrOfIndicesInBuffer;
		uint32_t* indexData;
	} indexInfo;

	struct SubMeshInfo
	{
		size_t startIndexValue;
		size_t nrOfIndicesInSubMesh;
		ID3D11ShaderResourceView* ambientTextureSRV;
		ID3D11ShaderResourceView* diffuseTextureSRV;
		ID3D11ShaderResourceView* specularTextureSRV;
	};

	std::vector<SubMeshInfo> subMeshInfo;
};

struct Material
{
	float ambientRGBA[4];
	float diffuseRGBA[4];
	float specularRGBA[4];
	float ambientIntensity;
	float specularPower;
	float padding;
	float padding2;

	Material()
	{
		for (int i = 0; i < 4; ++i)
		{
			ambientRGBA[i] = 1.0f;
			diffuseRGBA[i] = 1.0f;
			specularRGBA[i] = 1.0f;
		}

		ambientIntensity = 1.0f;
		padding = 0.0f;
		specularPower = 1.0f;
	}

	Material(const std::array<float, 4>& otherAmbient, const std::array<float, 4>& otherDiffuse, const std::array<float, 4>& otherSpecular, const float& otherAmbientIntensity, const float& otherPadding, const float& otherSpecularPower)
	{
		for (int i = 0; i < 4; ++i)
		{
			ambientRGBA[i] = otherAmbient[i];
			diffuseRGBA[i] = otherDiffuse[i];
			specularRGBA[i] = otherSpecular[i];
		}

		ambientIntensity = otherAmbientIntensity;
		padding = otherPadding;
		specularPower = otherSpecularPower;
	}
};

class MeshD3D11
{
private:
	MeshData meshData;
	std::vector<SubMeshD3D11> subMeshes;
	VertexBufferD3D11 vertexBuffer;
	IndexBufferD3D11 indexBuffer;

	Material materialData;
	ConstantBufferD3D11 materialBuffer;



	//ID3D11Buffer* materialBuffer = nullptr;

public:
	MeshD3D11() = default;
	~MeshD3D11() = default;
	MeshD3D11(const MeshD3D11 & other) = delete;
	MeshD3D11& operator=(const MeshD3D11 & other) = delete;
	MeshD3D11(MeshD3D11 && other) = delete;
	MeshD3D11& operator=(MeshD3D11 && other) = delete;

	void Initialize(ID3D11Device* device, const MeshData& meshInfo, const Material& materialData);

	void BindMeshBuffers(ID3D11DeviceContext* context) const;
	void PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const;

	void SetMaterialBuffer(ConstantBufferD3D11&& materialBuffer);

	size_t GetNrOfSubMeshes() const;
	ID3D11ShaderResourceView* GetAmbientSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetDiffuseSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetSpecularSRV(size_t subMeshIndex) const;
	std::string GetMeshName() const;
	UINT GetNrOfVertices() const;
	UINT GetVertexSize() const;
	UINT GetNrOfIndices() const;
	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;
	ID3D11Buffer* GetMaterialBuffer() const;
};