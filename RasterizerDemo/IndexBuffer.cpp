#include "IndexBufferD3D11.h"

IndexBufferD3D11::IndexBufferD3D11(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData) 
{

}

IndexBufferD3D11::~IndexBufferD3D11()
{	
	if (buffer) 
	{
		this->buffer->Release();
		this->buffer = nullptr;
	}

}

void IndexBufferD3D11::Initialize(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData)
{

	this->nrOfIndices = nrOfIndicesInBuffer;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(unsigned int) * nrOfIndices;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &this->buffer);
}

size_t IndexBufferD3D11::GetNrOfIndices() const
{
	return this->nrOfIndices;
}

ID3D11Buffer* IndexBufferD3D11::GetBuffer() const
{
	return this->buffer;
}