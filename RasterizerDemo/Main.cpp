#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <chrono>
#include <vector>

#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "IndexBufferD3D11.h"
#include "VertexBufferD3D11.h"

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv,
			ID3D11DepthStencilView* dsView, ID3D11DepthStencilState* dsState, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
			ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, VertexBufferD3D11& vertexBuffer, IndexBufferD3D11& indexBuffer, std::vector<unsigned int> indices)
{
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	ID3D11Buffer* buffers[] = { vertexBuffer.GetBuffer() };
	ID3D11Buffer* indexBuffers[] = { indexBuffer.GetBuffer() };
	immediateContext->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
	immediateContext->IASetIndexBuffer(*indexBuffers, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->OMSetDepthStencilState(dsState, 0);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);


	immediateContext->DrawIndexed(indexBuffer.GetNrOfIndices(), 0, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	ID3D11DepthStencilState* dsState;
	D3D11_VIEWPORT viewport;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	//ID3D11Buffer* vertexBuffer;
	//ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBufferVertex;
	ID3D11Buffer* constantLightBuffer;
	ID3D11Buffer* constantMaterialBuffer;
	ID3D11Buffer* constantCameraBuffer;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11Texture2D* gBuffer;
	ID3D11RenderTargetView* gBufferRtv;
	ID3D11ShaderResourceView* gBufferSrv;
	ID3D11SamplerState* samplerState;
;
	VertexBufferD3D11 testVertexBuffer;
	IndexBufferD3D11 testIndexBuffer;	


	std::vector<std::string> modelNames;

	modelNames.push_back("monkey.obj");

	std::vector<unsigned int> indices;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, dsState, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, testVertexBuffer, testIndexBuffer, vShader, pShader, inputLayout, constantBufferVertex, constantLightBuffer, constantMaterialBuffer, constantCameraBuffer, immediateContext, texture, srv, samplerState, modelNames,indices, gBuffer, gBufferRtv, gBufferSrv, WIDTH, HEIGHT))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	MSG msg = { };

	XMFLOAT4X4 float4x4Array;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	float rotationAmount = 0.0f;

	float xDist = 0.0f;

	//rendering loop
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Start time for the time to render a frame
		auto startTime = std::chrono::high_resolution_clock::now();

		// creation of the new world matrix for the rotation
		XMMATRIX newWorldMatrix = CreateWorldMatrix(rotationAmount, xDist);
		XMStoreFloat4x4(&float4x4Array, newWorldMatrix);

		// Mapping the new world matrix to the vertex shader
		immediateContext->Map(constantBufferVertex, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, &float4x4Array, sizeof(XMFLOAT4X4));
		immediateContext->Unmap(constantBufferVertex, 0);
		
		// Rendering
		Render(immediateContext, rtv, dsView, dsState, viewport, vShader, pShader, inputLayout, testVertexBuffer, testIndexBuffer, indices);
		swapChain->Present(0, 0);

		// End time for chorno for the time to render a frame and the total time to render a frame
		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
		
		// Turning the time into a float and determning the amount of time for 1 full rotation
		float timePerFrame = duration.count();
		float timeForRotation = 7.5f;
		
		// A check for to see if full rotation
		if (rotationAmount >= XM_PI*2)
		{
			rotationAmount = 0;
		}		
		
		if (xDist >= 1.0f)
		{
			xDist = 0.0f;
		}
		// Adaptivly adding rotation amount for each frame so that it will make a full rotation in a set amount of time
		rotationAmount += (timePerFrame/timeForRotation)*XM_2PI;
		xDist += (timePerFrame / timeForRotation) * 0.5f;
	}

	texture->Release();
	srv->Release();
	samplerState->Release();
	//vertexBuffer->Release();
	//indexBuffer->Release();
	constantBufferVertex->Release();
	constantLightBuffer->Release();
	constantMaterialBuffer->Release();
	constantCameraBuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
