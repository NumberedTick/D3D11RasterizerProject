#include "D3D11Helper.h"

bool CreateInterfaces(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, UINT width, UINT height, HWND window)
{
	UINT flags = 0;
	if (_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Default
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &immediateContext);
	
	return !(FAILED(hr));
}

bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv)
{
	// get the address of the back buffer
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cerr << "Failed to get back buffer!" << std::endl;
		return false;
	}

	// use the back buffer address to create the render target
	// null as description to base it on the backbuffers values
	HRESULT hr = device->CreateRenderTargetView(backBuffer, NULL, &rtv);
	backBuffer->Release();
	return !(FAILED(hr));

}

bool CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11UnorderedAccessView*& uav)
{
	ID3D11Texture2D* backBuffer = nullptr;

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cerr << "Failed to get back buffer!" << std::endl;
		return false;
	}

	HRESULT hr = device->CreateUnorderedAccessView(backBuffer, NULL, &uav);
	backBuffer->Release();
	return !(FAILED(hr));
}

bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture)))
	{
		std::cerr << "Failed to create depth stencil texture!" << std::endl;
		return false;
	}

	HRESULT hr = device->CreateDepthStencilView(dsTexture, 0, &dsView);
	return !(FAILED(hr));
}

bool CreateDepthStencilState(ID3D11Device* device, ID3D11DepthStencilState*& dsState)
{
	D3D11_DEPTH_STENCIL_DESC dsStateDesc;
	dsStateDesc.DepthEnable = true;
	dsStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsStateDesc.StencilEnable = true;
	dsStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr = device->CreateDepthStencilState(&dsStateDesc, &dsState);
	return !(FAILED(hr));
}

void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
}

bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device,
	ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11UnorderedAccessView*& uav,
	ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, ID3D11DepthStencilState*& dsState, D3D11_VIEWPORT& viewport)
{
	if (!CreateInterfaces(device, immediateContext, swapChain, width, height, window))
	{
		std::cerr << "Error creating interfaces!" << std::endl;
		return false;
	}

	
	if (!CreateRenderTargetView(device, swapChain, rtv))
	{
		std::cerr << "Error creating rtv!" << std::endl;
		return false;
	}
	
	if (!CreateUnorderedAccessView(device, swapChain, uav))
	{
		std::cerr << "Error creating rtv!" << std::endl;
		return false;
	}

	if (!CreateDepthStencil(device, width, height, dsTexture, dsView))
	{
		std::cerr << "Error creating depth stencil view!" << std::endl;
		return false;
	}

	if (!CreateDepthStencilState(device, dsState)) 
	{
		std::cerr << "Error creating depth stencil state!" << std::endl;
		return false;
	}

	SetViewport(viewport, width, height);

	return true;
}