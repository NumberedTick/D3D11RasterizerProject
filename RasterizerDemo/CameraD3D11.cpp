#include "CameraD3D11.h"

CameraD3D11::CameraD3D11(ID3D11Device* device, const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition)
{

}

void CameraD3D11::Initialize(ID3D11Device* device, const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition)
{
	
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

void CameraD3D11::RotateForward(float amount)
{

}

void CameraD3D11::RotateRight(float amount)
{

}

void CameraD3D11::RotateUp(float amount)
{

}

const DirectX::XMFLOAT3& CameraD3D11::GetPosition() const
{

}

const DirectX::XMFLOAT3& CameraD3D11::GetForward() const
{

}

const DirectX::XMFLOAT3& CameraD3D11::GetRight() const
{

}

const DirectX::XMFLOAT3& CameraD3D11::GetUp() const
{

}

void CameraD3D11::UpdateInternalConstantBuffer(ID3D11DeviceContext* context)
{

}

ID3D11Buffer* CameraD3D11::GetConstantBuffer() const
{

}

DirectX::XMFLOAT4X4 CameraD3D11::GetViewProjectionMatrix() const
{

}