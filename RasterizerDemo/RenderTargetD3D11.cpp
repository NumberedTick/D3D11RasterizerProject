#include "RenderTargetD3D11.h"

RenderTargetD3D11::~RenderTargetD3D11()
{

}

void RenderTargetD3D11::Initialize(ID3D11Device* device, UINT width, UINT height,
	DXGI_FORMAT format, bool hasSRV)
{

}

ID3D11RenderTargetView* RenderTargetD3D11::GetRTV() const
{
	return rtv;
}
ID3D11ShaderResourceView* RenderTargetD3D11::GetSRV() const
{
	return srv;
}