#include "CameraD3D11.h"
#include "ConstantBufferD3D11.h"
#include "PipelineHelper.h"
#include <DirectXMath.h>

CameraD3D11::CameraD3D11(ID3D11Device* device, const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition)
{
	this->position = initialPosition;
	this->projInfo = projectionInfo;

	Initialize(device, projectionInfo, initialPosition);
}

struct CameraData
{
	DirectX::XMFLOAT3 position;
	float padding;
};

void CameraD3D11::Initialize(ID3D11Device* device, const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition)
{
	this->position = initialPosition;
	this->projInfo = projectionInfo;
	
	CameraData data = { initialPosition, 0.0f };
	this->cameraBuffer = ConstantBufferD3D11(device, sizeof(CameraData), &data);
}

void CameraD3D11::MoveForward(float amount)
{

}

void CameraD3D11::MoveRight(float amount)
{

}

void CameraD3D11::MoveUp(float amount)
{

}

void CameraD3D11::RotateAroundAxis(float amount, const DirectX::XMFLOAT3& axis)
{
	DirectX::XMVECTOR rotationAxis = DirectX::XMLoadFloat3(&axis);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, amount);
	DirectX::XMVECTOR tempForward = DirectX::XMLoadFloat3(&this->GetForward());
	DirectX::XMVECTOR tempRight = DirectX::XMLoadFloat3(&this->GetRight());
	DirectX::XMVECTOR tempUp = DirectX::XMLoadFloat3(&this->GetUp());

	tempForward = DirectX::XMVector3Transform(tempForward, rotationMatrix);
	DirectX::XMStoreFloat3(&this->forward, tempForward);

	tempRight = DirectX::XMVector3Transform(tempRight, rotationMatrix);
	DirectX::XMStoreFloat3(&this->right, tempRight);

	tempUp = DirectX::XMVector3Transform(tempUp, rotationMatrix);
	DirectX::XMStoreFloat3(&this->up, tempUp);

}

void CameraD3D11::RotateForward(float amount)
{
	this->RotateAroundAxis(amount, this->GetForward());
}

void CameraD3D11::RotateRight(float amount)
{
	this->RotateAroundAxis(amount, this->GetRight());
}

void CameraD3D11::RotateUp(float amount)
{
	this->RotateAroundAxis(amount, this->GetUp());
}

const DirectX::XMFLOAT3& CameraD3D11::GetPosition() const
{
	return this->position;
}

const DirectX::XMFLOAT3& CameraD3D11::GetForward() const
{
	return this->forward;
}

const DirectX::XMFLOAT3& CameraD3D11::GetRight() const
{
	return this->right;
}

const DirectX::XMFLOAT3& CameraD3D11::GetUp() const
{
	return this->up;
}

void CameraD3D11::UpdateInternalConstantBuffer(ID3D11DeviceContext* context)
{

}

ID3D11Buffer* CameraD3D11::GetConstantBuffer() const
{
	return this->cameraBuffer.GetBuffer();
}

DirectX::XMFLOAT4X4 CameraD3D11::GetViewProjectionMatrix() const
{
	XMVECTOR viewVector = DirectX::XMLoadFloat3(&this->GetForward());
	XMVECTOR upDirection = DirectX::XMLoadFloat3(&this->GetUp());
	XMVECTOR eyePosition = DirectX::XMLoadFloat3(&this->GetPosition());
	float fovAngleY = this->projInfo.fovAngleY;
	float aspectRatio = this->projInfo.aspectRatio;
	float nearZ = this->projInfo.nearZ;
	float farZ = this->projInfo.farZ;

	DirectX::XMMATRIX cameraViewProjectionMatrix = CreatViewPerspectiveMatrix(viewVector, upDirection, eyePosition, fovAngleY, aspectRatio, nearZ, farZ);

	DirectX::XMFLOAT4X4 returnFloat4X4;

	DirectX::XMStoreFloat4x4(&returnFloat4X4, cameraViewProjectionMatrix);


	return returnFloat4X4;
}
