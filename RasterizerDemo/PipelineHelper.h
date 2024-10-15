#pragma once

#include <array>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <iostream>

#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "CameraD3D11.h"

using namespace DirectX;

struct SimpleVertex
{
	float pos[3];
	float norm[3];
	float UV[2];

	SimpleVertex()
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = 0;
			norm[i] = 0;
		}
		for (int j = 0; j < 2; ++j)
		{
			UV[j] = 0;
		}
	}

	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 3>& normal, const std::array<float, 2>& uv)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			norm[i] = normal[i];
		}
		for (int j = 0; j < 2; ++j) 
		{
			UV[j] = uv[j];
		}
	}
};

struct MatrixBuffer
{
	XMFLOAT4X4 float4x4Matrix1;
	XMFLOAT4X4 float4x4Matrix2;

	MatrixBuffer(const XMMATRIX& m1, const XMMATRIX& m2, XMFLOAT4X4* float4x4Array)
	{
		XMStoreFloat4x4(float4x4Array, m1);
		XMStoreFloat4x4(float4x4Array + 1, m2);
		float4x4Matrix1 = float4x4Array[0];
		float4x4Matrix2 = float4x4Array[1];
	}
};

struct ConstantBuffer
{
	XMFLOAT4X4 constantWorldMatrix;
	XMFLOAT4X4 constantViewMatrix;

	ConstantBuffer(float angle)
	{
		XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.0f);
		XMMATRIX rotationMatrix = XMMatrixRotationY(angle);
		XMMATRIX worldMatrix = XMMatrixMultiply(translationMatrix, rotationMatrix);


		XMFLOAT4X4 tempFloat4x4Array[2];
		XMStoreFloat4x4(tempFloat4x4Array, worldMatrix);

	}
};

struct PointLight 
{
	float lightPosition[4];
	float lightColor[4];
	float lightIntencity;

	PointLight(const std::array<float, 4>& otherLightPosition, const std::array<float, 4>& otherLightColor, const float& otherLightIntensity)
	{
		for (int i = 0; i < 4; ++i) 
		{
			lightPosition[i] = otherLightPosition[i];
			lightColor[i] = otherLightColor[i];
		}

		lightIntencity = otherLightIntensity;
	}
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
			ambientRGBA[i] = 0.0f;
			diffuseRGBA[i] = 0.0f;
			specularRGBA[i] = 0.0f;
		}

		ambientIntensity = 0.0f;
		padding = 0.0f;
		specularPower = 0.0f;
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


template <typename T>
class QuadTree
{
private:
	struct Node
	{
		T element;
		std::unique_ptr<Node> children[4];
	};

	std::unique_ptr<Node> root;
};


XMMATRIX CreateWorldMatrix(float angle, float xDist, float yDist, float zDist);

XMMATRIX CreatViewPerspectiveMatrix(XMVECTOR viewVector, XMVECTOR upDirection, XMVECTOR eyePosition, float fovAngleY, float aspectRatio, float nearZ, float farZ);

bool Create2DTexture(ID3D11Device* device, ID3D11Texture2D*& texture, std::string textureName);

bool CreateSRV(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv);

bool CreateGBuffer(ID3D11Device* device, ID3D11Texture2D*& gBuffer, ID3D11RenderTargetView*& gBufferRtv, ID3D11ShaderResourceView*& gBufferSrv, UINT width, UINT height);

bool CreateTextureCube(ID3D11Device* device, ID3D11Texture2D*& cubeMapTexture, ID3D11UnorderedAccessView**& cubeMapUavArray, ID3D11ShaderResourceView*& cubeMapSRV);

bool SetupPipeline(ID3D11Device* device, VertexBufferD3D11**& vertexBuffer, IndexBufferD3D11**& indexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11PixelShader*& pShaderCubeMap,
	ID3D11ComputeShader*& cShader,ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantWorldMatrixBuffer, ID3D11Buffer*& constantViewProjMatrixBuffer, ID3D11Buffer*& constantLightPixel, 
	ID3D11Buffer*& constantMaterialBuffer, ID3D11Buffer*& constantCameraBuffer, ID3D11DeviceContext*& deviceContext, ID3D11Texture2D*& cubeMapTexture, ID3D11UnorderedAccessView**& cubeMapUavArray,
	ID3D11ShaderResourceView*& cubeMapSrv, CameraD3D11**& cameraArray, D3D11_VIEWPORT& cubeMapViewport, ID3D11Texture2D*& cubeMapDSTexture, ID3D11DepthStencilView*& cubeMapDSView, ID3D11DepthStencilState*& cubeMapDSState, 
	ID3D11SamplerState*& sampleState, std::vector<std::string>& modelNames, UINT width, UINT height, Material**& materialArray, ConstantBufferD3D11**& materialBufferArray, ID3D11UnorderedAccessView*& uavTextureCube);