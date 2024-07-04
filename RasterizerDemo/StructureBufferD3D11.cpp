#include "StructuredBufferD3D11.h"

StructuredBufferD3D11::StructuredBufferD3D11(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer, void* bufferData, bool dynamic)
{

}

StructuredBufferD3D11::~StructuredBufferD3D11()
{

}

void StructuredBufferD3D11::Initialize(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer, void* bufferData, bool dynamic)
{

}

void StructuredBufferD3D11::UpdateBuffer(ID3D11DeviceContext* context, void* data)
{

}
UINT StructuredBufferD3D11::GetElementSize() const
{
	return elementSize;
}

size_t StructuredBufferD3D11::GetNrOfElements() const
{
	return nrOfElements;
}

ID3D11ShaderResourceView* StructuredBufferD3D11::GetSRV() const
{
	return srv;
}