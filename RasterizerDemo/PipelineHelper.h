#pragma once

#include <array>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

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
	float padding;
	float specularPower;
	float padding2;

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


XMMATRIX CreateWorldMatrix(float angle, float xDist);

XMMATRIX CreatViewPerspectiveMatrix(float xPos, float yPos, float zPos);

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBufferVertex, ID3D11Buffer*& constantLightPixel, ID3D11Buffer*& constantMaterialBuffer, ID3D11Buffer*& constantCameraBuffer, ID3D11DeviceContext*& deviceContext, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& sampleState, std::vector<unsigned int>& indices, ID3D11Texture2D*& gBuffer, ID3D11RenderTargetView*& gBufferRtv, ID3D11ShaderResourceView*& gBufferSrv, UINT width, UINT height);
