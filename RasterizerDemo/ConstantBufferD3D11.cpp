#include "ConstantBufferD3D11.h"

ConstantBufferD3D11::ConstantBufferD3D11(ID3D11Device* device, size_t byteSize, void* initialData)
{

}

ConstantBufferD3D11::~ConstantBufferD3D11()
{

}

ConstantBufferD3D11::ConstantBufferD3D11(ConstantBufferD3D11&& other)
{

}

ConstantBufferD3D11& ConstantBufferD3D11::operator=(ConstantBufferD3D11&& other)
{

}

void ConstantBufferD3D11::Initialize(ID3D11Device* device, size_t byteSize, void* initialData)
{

}

size_t ConstantBufferD3D11::GetSize() const
{
	return bufferSize;
}

ID3D11Buffer* ConstantBufferD3D11::GetBuffer() const
{
	return buffer;
}

void ConstantBufferD3D11::UpdateBuffer(ID3D11DeviceContext* context, void* data)
{

}