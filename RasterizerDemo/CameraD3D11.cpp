#include "CameraD3D11.h"
#include "ConstantBufferD3D11.h"
#include "PipelineHelper.h"
#include <DirectXMath.h>

CameraD3D11::CameraD3D11(ID3D11Device* device, const ProjectionInfo& projectionInfo, const XMFLOAT3& initialPosition)
{
	//this->position = initialPosition;
	//this->projInfo = projectionInfo;

	Initialize(device, projectionInfo, initialPosition);
}


void CameraD3D11::Initialize(ID3D11Device* device, const ProjectionInfo& projectionInfo, const XMFLOAT3& initialPosition)
{
	this->position = initialPosition;
	this->projInfo = projectionInfo;
	
	//this->SetViewProjectionMatrix();

	XMFLOAT4X4 cameraViewProjMatrix = this->SetViewProjectionMatrix();
	

	this->cameraBuffer = ConstantBufferD3D11(device, sizeof(XMFLOAT4X4), &cameraViewProjMatrix);
}

void CameraD3D11::MoveForward(float amount)
{
	this->MoveInDirection(amount, this->GetForward());
}

void CameraD3D11::MoveRight(float amount)
{
	this->MoveInDirection(amount, this->GetRight());
}

void CameraD3D11::MoveUp(float amount)
{
	this->MoveInDirection(amount, this->GetUp());
}

void CameraD3D11::MoveInDirection(float amount, const XMFLOAT3& direction)
{
	XMVECTOR directionVector = XMLoadFloat3(&direction);
	XMVECTOR tempForward = XMLoadFloat3(&this->GetForward());
	XMVECTOR tempRight = XMLoadFloat3(&this->GetRight());
	XMVECTOR tempUp = XMLoadFloat3(&this->GetUp());
	XMVECTOR positionVector = XMLoadFloat3(&this->GetPosition());
	
	XMVECTOR scaledVector = XMVectorScale(tempForward, amount);
	scaledVector = XMVectorScale(tempRight, amount);
	scaledVector = XMVectorScale(tempUp, amount);

	XMStoreFloat3(&this->position, XMVectorAdd(positionVector, scaledVector));
}

void CameraD3D11::RotateAroundAxis(float amount, const XMFLOAT3& axis)
{
	XMVECTOR rotationAxis = XMLoadFloat3(&axis);
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, amount);
	XMVECTOR tempForward = XMLoadFloat3(&this->GetForward());
	XMVECTOR tempRight = XMLoadFloat3(&this->GetRight());
	XMVECTOR tempUp = XMLoadFloat3(&this->GetUp());

	tempForward = XMVector3Transform(tempForward, rotationMatrix);
	XMStoreFloat3(&this->forward, tempForward);

	tempRight = XMVector3Transform(tempRight, rotationMatrix);
	XMStoreFloat3(&this->right, tempRight);

	tempUp = XMVector3Transform(tempUp, rotationMatrix);
	XMStoreFloat3(&this->up, tempUp);
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

const XMFLOAT3& CameraD3D11::GetPosition() const
{
	return this->position;
}

void CameraD3D11::Setposition(XMFLOAT3 pos)
{
	this->position = pos;
	this->viewProjmatrix = this->SetViewProjectionMatrix();
}

const XMFLOAT3& CameraD3D11::GetForward() const
{
	return this->forward;
}

const XMFLOAT3& CameraD3D11::GetRight() const
{
	return this->right;
}

const XMFLOAT3& CameraD3D11::GetUp() const
{
	return this->up;
}

void CameraD3D11::UpdateInternalConstantBuffer(ID3D11DeviceContext* context)
{
	XMFLOAT4X4 data = this->SetViewProjectionMatrix();
	this->cameraBuffer.UpdateBuffer(context, &data);
}

ID3D11Buffer* CameraD3D11::GetConstantBuffer() const
{
	return this->cameraBuffer.GetBuffer();
}

XMFLOAT4X4 CameraD3D11::SetViewProjectionMatrix() 
{
	XMVECTOR viewVector = XMLoadFloat3(&this->GetForward());
	XMVECTOR upDirection = XMLoadFloat3(&this->GetUp());
	XMVECTOR eyePosition = XMLoadFloat3(&this->GetPosition());
	float fovAngleY = this->projInfo.fovAngleY;
	float aspectRatio = this->projInfo.aspectRatio;
	float nearZ = this->projInfo.nearZ;
	float farZ = this->projInfo.farZ;

	XMMATRIX cameraViewProjectionMatrix = CreatViewPerspectiveMatrix(viewVector, upDirection, eyePosition, fovAngleY, aspectRatio, nearZ, farZ);

	XMFLOAT4X4 storedFloat4X4;

	XMStoreFloat4x4(&storedFloat4X4, cameraViewProjectionMatrix);

	// returning the viewProjectionMatrix was needed more then expected
	this->viewProjmatrix = storedFloat4X4;
	return storedFloat4X4; 
}


XMFLOAT4X4 CameraD3D11::GetViewProjectionMatrix() const
{
	return this->viewProjmatrix;
}
