#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>

void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height);

bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView);

bool CreateDepthStencilState(ID3D11Device* device, ID3D11DepthStencilState*& dsState);

bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device *& device, ID3D11DeviceContext *& immediateContext, IDXGISwapChain *& swapChain, ID3D11RenderTargetView *& rtv,ID3D11UnorderedAccessView*& uav, ID3D11Texture2D *& dsTexture, ID3D11DepthStencilView *& dsView, ID3D11DepthStencilState*& dsState, D3D11_VIEWPORT & viewport);
