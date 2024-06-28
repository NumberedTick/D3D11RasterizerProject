#include "VertexBufferD3D11.h"
#include <stdexcept>

VertexBufferD3D11::VertexBufferD3D11(ID3D11Device* device, UINT sizeOfVertex,
	UINT nrOfVerticesInBuffer, void* vertexData)
{
	this->nrOfVertices = nrOfVerticesInBuffer;

	this->vertexSize = sizeOfVertex;

	Initialize(device, sizeOfVertex, nrOfVerticesInBuffer, vertexData);
}

VertexBufferD3D11::~VertexBufferD3D11()
{
	this->buffer->Release();
	this->buffer = nullptr;
}

void VertexBufferD3D11::Initialize(ID3D11Device* device, UINT sizeOfVertex,
	UINT nrOfVerticesInBuffer, void* vertexData)
{
	this->nrOfVertices = nrOfVerticesInBuffer;

	this->vertexSize = sizeOfVertex;

	if (this->GetBuffer())
	{
		this->buffer->Release();
		this->buffer = nullptr;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeOfVertex * nrOfVerticesInBuffer;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;


	// Data for the bufferDesc
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &this->buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Error Creating Vertex Buffer");
	}


}

UINT VertexBufferD3D11::GetNrOfVertices() const
{
	return this->nrOfVertices;
}

UINT VertexBufferD3D11::GetVertexSize() const
{
	return this->vertexSize;
}

ID3D11Buffer* VertexBufferD3D11::GetBuffer() const
{
	return this->buffer;
}