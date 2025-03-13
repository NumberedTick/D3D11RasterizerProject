#include "ConstantBufferD3D11.h"

ConstantBufferD3D11::ConstantBufferD3D11(ID3D11Device* device, size_t byteSize, void* initialData)
{
	this->bufferSize = byteSize;

	Initialize(device, byteSize, initialData);
}

ConstantBufferD3D11::~ConstantBufferD3D11()
{
	if (buffer)
	{
		this->buffer->Release();
		this->buffer = nullptr;
	}
}

ConstantBufferD3D11::ConstantBufferD3D11(ConstantBufferD3D11&& other) noexcept
	: buffer(other.buffer), bufferSize(other.bufferSize)
{
	other.buffer = nullptr;
	other.bufferSize = 0;
}

ConstantBufferD3D11& ConstantBufferD3D11::operator=(ConstantBufferD3D11&& other) noexcept
{
	if (this != &other)
	{
		if (buffer)
		{
			this->buffer->Release();
		}
		this->buffer = other.buffer;
		this->bufferSize = other.bufferSize;

		other.buffer = nullptr;
		other.bufferSize = 0;
	}
	return *this;
}

void ConstantBufferD3D11::Initialize(ID3D11Device* device, size_t byteSize, void* initialData)
{
	this->bufferSize = byteSize;

	if (this->GetBuffer())
	{
		this->buffer->Release();
		this->buffer = nullptr;
	}


	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = byteSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;


	// Data for the bufferDesc
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &this->buffer);
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

	//context->UpdateSubresource(this->buffer, 0, nullptr, data, 0, 0);

	
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(this->buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	memcpy(mappedResource.pData, data, this->bufferSize);
	context->Unmap(this->buffer, 0);
	
	
}