#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>

#include "ConstantBufferD3D11.h"

struct ProjectionInfo
{
	float fovAngleY = 0.0f;
	float aspectRatio = 0.0f;
	float nearZ = 0.0f;
	float farZ = 0.0f;
};

class CameraD3D11
{
private:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 forward = { 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	ProjectionInfo projInfo;
	DirectX::XMFLOAT4X4 viewProjmatrix;

	// create buffer for viewProjectionMatruxBuffer
	ConstantBufferD3D11 cameraBuffer;

	void MoveInDirection(float amount, const DirectX::XMFLOAT3& direction);
	void RotateAroundAxis(float amount, const DirectX::XMFLOAT3& axis);

public:
	CameraD3D11() = default;
	CameraD3D11(ID3D11Device* device, const ProjectionInfo& projectionInfo,
		const DirectX::XMFLOAT3& initialPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	~CameraD3D11() = default;
	CameraD3D11(const CameraD3D11& other) = delete;
	CameraD3D11& operator=(const CameraD3D11& other) = delete;
	CameraD3D11(CameraD3D11&& other) = default;
	CameraD3D11& operator=(CameraD3D11&& other) = default;

	void Initialize(ID3D11Device* device, const ProjectionInfo& projectionInfo,
		const DirectX::XMFLOAT3& initialPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT3& forwardVector = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),
		const DirectX::XMFLOAT3& upVector = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveUp(float amount);

	void RotateForward(float amount);
	void RotateRight(float amount);
	void RotateUp(float amount);

	const DirectX::XMFLOAT3& GetPosition() const;

	void Setposition(DirectX::XMFLOAT3 pos); // temp function to set position for debugging

	void SetForward(DirectX::XMFLOAT3 forward);
	void SetRight(DirectX::XMFLOAT3 right);
	void SetUp(DirectX::XMFLOAT3 up);

	const DirectX::XMFLOAT3& GetForward() const;
	const DirectX::XMFLOAT3& GetRight() const;
	const DirectX::XMFLOAT3& GetUp() const;

	void UpdateInternalConstantBuffer(ID3D11DeviceContext* context);
	ID3D11Buffer* GetConstantBuffer() const;

	// update the viewProjectionMatrixBuffer
	// get the viewProjectionMatrixBuffer

	DirectX::XMFLOAT4X4 SetViewProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix() const;
};