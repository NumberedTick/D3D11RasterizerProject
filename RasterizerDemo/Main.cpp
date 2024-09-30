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
#include "CameraD3D11.h"

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView** rtvArr,
			ID3D11DepthStencilView* dsView, ID3D11DepthStencilState* dsState, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
			ID3D11PixelShader* pShader, ID3D11ComputeShader* cShader,ID3D11InputLayout* inputLayout, VertexBufferD3D11* vertexBuffer, 
			IndexBufferD3D11* indexBuffer, ID3D11Buffer* tempConstantBuffer, ID3D11Buffer* viewProjBuffers, ID3D11Buffer* bufferArray[3], ConstantBufferD3D11*& materialBufferArray)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	ID3D11Buffer* buffers[] = { vertexBuffer[0].GetBuffer()};
	ID3D11Buffer* indexBuffers[] = { indexBuffer[0].GetBuffer()};
	ID3D11Buffer* materialBuffer = materialBufferArray->GetBuffer();
	immediateContext->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
	immediateContext->IASetIndexBuffer(*indexBuffers, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->OMSetDepthStencilState(dsState, 0);
	immediateContext->VSSetShader(vShader, nullptr, 0);	
	immediateContext->VSSetConstantBuffers(0, 1, &tempConstantBuffer);
	immediateContext->VSSetConstantBuffers(1, 1, &viewProjBuffers);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &materialBuffer);
	immediateContext->OMSetRenderTargets(6, rtvArr, dsView);


	immediateContext->DrawIndexed(indexBuffer[0].GetNrOfIndices(), 0, 0);

	immediateContext->VSSetConstantBuffers(0, 0, nullptr);
}


void RenderReflectivObject(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView** rtvArr,
	ID3D11DepthStencilView* dsView, ID3D11DepthStencilState* dsState, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader, ID3D11ComputeShader*& cShader, ID3D11UnorderedAccessView**& cubeMapUavArray,ID3D11InputLayout* inputLayout, VertexBufferD3D11*& vertexBuffer, 
	IndexBufferD3D11*& indexBuffer, CameraD3D11** cubeMapCameras,  ID3D11Buffer* worldMatrixBuffer,D3D11_MAPPED_SUBRESOURCE mappedResource,
	ConstantBufferD3D11*& materialBufferArray, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView** gBufferCubeMapSRV)
{
	
	//immediateContext->PSSetShaderResources(0, 1, &nullSRV);


	//ID3D11ShaderResourceView* srvArr[6];
	ID3D11RenderTargetView* nullRTV = nullptr;

	//srvArr = gBufferCubeMapSRV[0];


	ID3D11Buffer* materialBuffer = materialBufferArray->GetBuffer();
	immediateContext->PSSetConstantBuffers(0, 1, &materialBuffer);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	ID3D11Buffer* buffers[] = { vertexBuffer[0].GetBuffer() };
	ID3D11Buffer* indexBuffers[] = { indexBuffer[0].GetBuffer() };

	float clearColour[4] = { 0, 0, 0, 0 };


	for (int i = 0; i < 6; ++i) // Render relevant objects for each of the six sides in the texture cube. 
	{


		for (int j = 0; j < 6; ++j)
		{
			immediateContext->ClearRenderTargetView(rtvArr[j], clearColour);
		}
			
		

		//cubeMapCameras[i]->UpdateInternalConstantBuffer(immediateContext); does not work, access violation
		XMFLOAT4X4 tempFloat4X4 = cubeMapCameras[i]->GetViewProjectionMatrix();
		cubeMapCameras[i]->UpdateInternalConstantBuffer(immediateContext);
		ID3D11Buffer* currentBuffer = cubeMapCameras[i]->GetConstantBuffer();
		immediateContext->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
		immediateContext->IASetIndexBuffer(*indexBuffers, DXGI_FORMAT_R32_UINT, 0);
		immediateContext->IASetInputLayout(inputLayout);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediateContext->OMSetDepthStencilState(dsState, 0);
		immediateContext->VSSetShader(vShader, nullptr, 0);
		immediateContext->VSSetConstantBuffers(0, 1, &worldMatrixBuffer);
		immediateContext->RSSetViewports(1, &viewport);
		immediateContext->PSSetShader(pShader, nullptr, 0);

		immediateContext->VSSetConstantBuffers(1, 1, &currentBuffer);
		
		immediateContext->OMSetRenderTargets(6, rtvArr, dsView);

		immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		

		immediateContext->DrawIndexed(indexBuffer[0].GetNrOfIndices(), 0, 0);


		immediateContext->OMSetRenderTargets(1, &nullRTV, nullptr);

		immediateContext->CSSetShader(cShader, nullptr, 0);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &cubeMapUavArray[i], nullptr);
		immediateContext->CSSetShaderResources(0, 6, gBufferCubeMapSRV);
		immediateContext->Dispatch(1024 / 8, 1024 / 8, 1);
		//immediateContext->VSSetConstantBuffers(1, 0, nullptr);
		
	}
	immediateContext->VSSetConstantBuffers(0, 0, nullptr);
	immediateContext->OMSetRenderTargets(1, &nullRTV, nullptr); 
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

	// Enable/disable cubemaps
	bool DynamicCubeMapsEnabled = true;

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
	ID3D11ComputeShader* cShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantWorldMatrixBuffer;
	ID3D11Buffer* constantViewProjMatrixBuffer;
	ID3D11Buffer* constantLightBuffer;
	ID3D11Buffer* constantMaterialBuffer;
	ID3D11Buffer* constantCameraBuffer;

	ID3D11SamplerState* samplerState;
	ID3D11UnorderedAccessView* uav;
	ID3D11UnorderedAccessView* uavTextureCube;



	// creation of the needed things for the cubemap
	// UPDATE CreateTextureCube(...) to use these resorces instead 
	ID3D11Texture2D* cubeMapTexture;
	ID3D11UnorderedAccessView** cubeMapUavArray = new ID3D11UnorderedAccessView * [6];
	ID3D11ShaderResourceView* cubeMapSrv;

	CameraD3D11** cubeMapCameras = new CameraD3D11 * [6];

	for (int i = 0; i < 6; ++i)
	{
		cubeMapCameras[i] = new CameraD3D11;
	}

	D3D11_VIEWPORT cubeMapViewport;
	ID3D11Texture2D* cubeMapDSTexture;
	ID3D11DepthStencilView* cubeMapDSView;
	ID3D11DepthStencilState* cubeMapDSState;
	


	// Loads Models into the scene
	std::vector<std::string> modelNames;


	modelNames.push_back("roomHoles.obj");
	modelNames.push_back("untitled.obj");
	modelNames.push_back("torus.obj");

	//modelNames.push_back("monkey.obj");
	//modelNames.push_back("untitled1.obj");

	UINT nrModels = static_cast<UINT>(modelNames.size());

	// Loads textures

	std::vector<std::string> textureNames;
	textureNames.push_back("texture2.png");
	textureNames.push_back("texture.jpg");
	textureNames.push_back("torus.png");

	Material** materialArray = new Material * [nrModels]; // MEMORY LEAK

	ConstantBufferD3D11** materialBufferArray = new ConstantBufferD3D11 * [nrModels]; // MEMORY LEAK

	std::string missingTexture = "missing.jpg";
	// Creates VertexBuffers for each model loaded (currently doing it manually)
	VertexBufferD3D11** vBuffer = new VertexBufferD3D11 * [nrModels];

	IndexBufferD3D11** iBuffer = new IndexBufferD3D11 * [nrModels];

	for (int i = 0; i < nrModels; ++i)
	{
		vBuffer[i] = new VertexBufferD3D11;
		iBuffer[i] = new IndexBufferD3D11;
		materialArray[i] = new Material; // MEMORY LEAK
		materialBufferArray[i] = new ConstantBufferD3D11; // MEMORY LEAK
	}

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, uav, dsTexture, dsView, dsState, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

		// Creationg of the GBuffers

	const unsigned int nrOfGBuffers = 6;

	ID3D11Texture2D** gBuffer = new ID3D11Texture2D * [nrOfGBuffers];
	ID3D11ShaderResourceView** gBufferSrv = new ID3D11ShaderResourceView * [nrOfGBuffers];
	ID3D11RenderTargetView** gBufferRtv = new ID3D11RenderTargetView * [nrOfGBuffers];
	ID3D11RenderTargetView* rtvArr[nrOfGBuffers];
	ID3D11ShaderResourceView* srvArr[nrOfGBuffers];

	/*
	if (DynamicCubeMapsEnabled)
	{
		if (!CreateTextureCube(device, cubeMapTexture, cubeMapUavArray, cubeMapSrv))
		{
			std::cerr << "Error creating G-Buffer for Texture Cube!" << std::endl;
			return false;
		}
	}
	*/
	// 
	// REMOVE, MOST LIKELY NOT NEEDED
	//ID3D11RenderTargetView** gBufferTextureRtvOLD = new ID3D11RenderTargetView* [nrOfGBuffers*6];
	//ID3D11RenderTargetView* cubeMapRtvArrOLD[nrOfGBuffers*6];
	// REMOVE, MOST LIKELY NOT NEEDED

	// texture cube buffers
	ID3D11Texture2D** gTextureBuffer = new ID3D11Texture2D * [nrOfGBuffers];
	ID3D11RenderTargetView** gBufferTextureRtv = new ID3D11RenderTargetView* [nrOfGBuffers];
	ID3D11ShaderResourceView** gBufferCubeMapSRV = new ID3D11ShaderResourceView * [nrOfGBuffers];
	ID3D11RenderTargetView* cubeMapRtvGBufferArr[nrOfGBuffers];
	ID3D11ShaderResourceView* cubeMapSrvGBufferArr[nrOfGBuffers];

	


	for (int i = 0; i < nrOfGBuffers; ++i)
	{
		if (!CreateGBuffer(device, gBuffer[i], gBufferRtv[i], gBufferSrv[i], WIDTH, HEIGHT))
		{
			std::cerr << "Error creating G-Buffer!" << std::endl;
			return false;
		}
		

		rtvArr[i] = gBufferRtv[i];
		srvArr[i] = gBufferSrv[i];


		// CHANGE TO ONLY CREATE 6 TEXTRURES OF SQUARE SIZE, 6 RTVS AND 6 SRVS
		if (DynamicCubeMapsEnabled)
		{

			if (!CreateGBuffer(device, gTextureBuffer[i], gBufferTextureRtv[i], gBufferCubeMapSRV[i], 1024, 1024))
			{
				std::cerr << "Error creating G-Buffer for Texture Cube!" << std::endl;
				return false;
			}

			cubeMapRtvGBufferArr[i] = gBufferTextureRtv[i];
			cubeMapSrvGBufferArr[i] = gBufferCubeMapSRV[i];

			/* REMOVE REMOVE REMOVE
			if (!CreateTextureCube(device, gTextureBuffer[i], gBufferTextureRtvOLD, gBufferCubeMapSRV[i]))
			{
				std::cerr << "Error creating g-buffers for Texture Cube";
				return false;
			}
			
			for (int j = 0; j < 6; ++j)
			{
				cubeMapRtvArrOLD[i * 6 + j] = gBufferTextureRtvOLD[i * 6 + j];
			}
			gBufferCubeMapSRV[i] = gBufferCubeMapSRV[i];
			*/
			
		}
		// CHANGE TO ONLY CREATE 6 TEXTRURES OF SQUARE SIZE, 6 RTVS AND 6 SRVS
	}


	if (!SetupPipeline(device, vBuffer, iBuffer, vShader, pShader, cShader,inputLayout, 
		constantWorldMatrixBuffer, constantViewProjMatrixBuffer, constantLightBuffer, constantMaterialBuffer,
		constantCameraBuffer, immediateContext, cubeMapTexture, cubeMapUavArray,
		cubeMapSrv, cubeMapCameras,cubeMapViewport, cubeMapDSTexture,cubeMapDSView,cubeMapDSState,
		samplerState, modelNames, WIDTH, HEIGHT, materialArray, materialBufferArray, uavTextureCube))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}


	
	// Creation of the needed textures for each model
	ID3D11Texture2D** modelTextures = new ID3D11Texture2D* [nrModels];
	ID3D11ShaderResourceView** srvModelTextures = new ID3D11ShaderResourceView* [nrModels];
	for (int i = 0; i < nrModels; ++i)
	{
		if (i < textureNames.size()) 
		{
			if (!Create2DTexture(device, modelTextures[i], textureNames[i]))
			{
				std::cerr << "Failed to create 2DTexture for model!" << std::endl;
				return -1;
			}

			if (!CreateSRV(device, modelTextures[i], srvModelTextures[i]))
			{
				std::cerr << "Failed to create SRV for model texture!" << std::endl;
				return -1;
			}
		}
		else // only used to create missing texture if there are more models than textures,
		{
			if (!Create2DTexture(device, modelTextures[i], missingTexture))
			{
				std::cerr << "Failed to create 2DTexture for missing texture!" << std::endl;
				return -1;
			}

			if (!CreateSRV(device, modelTextures[i], srvModelTextures[i]))
			{
				std::cerr << "Failed to create SRV for model texture!" << std::endl;
				return -1;
			}
		}
		
	}
	
	


	MSG msg = { };

	XMFLOAT4X4 float4x4Array[3];

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	float rotationAmount = 0.0f;

	float xDist = 2.0f;

	float yDist = 0.0f;

	float zDist = 0.5f;

	float clearColour[4] = { 0, 0, 0, 0 };

	XMMATRIX cubemapMatrix;

	// TEMP VALAUES USED UNTIL I SWITCH MAIN CAMERA TO A CAMERA OBJECT

	XMMATRIX baselineWorldMatrix = CreateWorldMatrix(XM_PIDIV4, 0.0f, 0.0f, -0.5f);
	XMStoreFloat4x4(&float4x4Array[1], baselineWorldMatrix);

	XMMATRIX newWorldMatrix = CreateWorldMatrix(XM_PIDIV2, -xDist, 0.0f, -0.5f);
	XMStoreFloat4x4(&float4x4Array[2], newWorldMatrix);

	XMVECTOR eyePosition = { 0.0f, 0.0f, -3.5f };
	XMVECTOR viewVecotr = { 0.0f, 0.0f, 1.0f };
	XMVECTOR upDirection = { 0.0f, 1.0f, 0.0f };
	float fovAgnleY = XM_PI / 2.5f;
	float aspectRatio = 1024.0f / 576.0f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	ConstantBufferD3D11 worldMatrixBuffer(device, sizeof(XMFLOAT4X4), &baselineWorldMatrix);

	ID3D11Buffer* tempBuffer;
	tempBuffer = worldMatrixBuffer.GetBuffer();	
	
	ConstantBufferD3D11 newWorldMatrixBuffer(device, sizeof(XMFLOAT4X4), &newWorldMatrix);

	ID3D11Buffer* tempBuffer2;
	tempBuffer2 = newWorldMatrixBuffer.GetBuffer();

	ID3D11Buffer** tempBufferArray[3];
	tempBufferArray[0] = &tempBuffer;
	tempBufferArray[1] = &constantWorldMatrixBuffer;
	tempBufferArray[2] = &tempBuffer2;

	ID3D11Buffer* bufferArray[3] = { constantLightBuffer, constantMaterialBuffer, constantCameraBuffer };


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
		newWorldMatrix = CreateWorldMatrix(rotationAmount, xDist, yDist, zDist);
		XMStoreFloat4x4(&float4x4Array[0], newWorldMatrix);

		// Mapping the new world matrix to the vertex shader
		immediateContext->Map(constantWorldMatrixBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, &float4x4Array, sizeof(XMFLOAT4X4));
		immediateContext->Unmap(constantWorldMatrixBuffer, 0);

		// Rendering part

		// Rendering the dynamic cube map
		if (DynamicCubeMapsEnabled)
		{
			immediateContext->PSSetShaderResources(0, 1, &srvModelTextures[0]);
			RenderReflectivObject(immediateContext, cubeMapRtvGBufferArr, cubeMapDSView, cubeMapDSState, 
				cubeMapViewport, vShader, pShader, cShader, cubeMapUavArray ,inputLayout, vBuffer[0], iBuffer[0], 
				cubeMapCameras, &tempBuffer[0], mappedResource, materialBufferArray[0], uavTextureCube, gBufferCubeMapSRV);
		}

		// Cleararing from last frame of main rendering
		for (int i = 0; i < nrOfGBuffers; ++i)
		{
			immediateContext->ClearRenderTargetView(rtvArr[i], clearColour);
		}
		immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		//immediateContext->ClearRenderTargetView(rtv, clearColour);
		


		// Gemoetry pass
		for (int i = 0; i < nrModels; ++i)
		{
			immediateContext->PSSetShaderResources(0, 1, &srvModelTextures[i]);
			Render(immediateContext, rtvArr, dsView, dsState, 
				viewport, vShader, pShader, cShader ,inputLayout, 
				vBuffer[i], iBuffer[i], *tempBufferArray[i], constantViewProjMatrixBuffer, 
				bufferArray, materialBufferArray[i]);
		}
		// Unbinding GBuffer RTVs
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		immediateContext->OMSetRenderTargets(1, nullRTV, nullptr);

		// Shading pass
		immediateContext->CSSetShader(cShader, nullptr, 0);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		immediateContext->CSSetShaderResources(0, 6, srvArr);
		immediateContext->Dispatch(WIDTH / 8, HEIGHT / 8, 1);


		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

		immediateContext->CSSetShaderResources(0, 1, nullSRV);

		swapChain->Present(0, 0);

		// End time for chorno for the time to render a frame and the total time to render a frame
		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
		
		// Turning the time into a float and determning the amount of time for 1 full rotation
		float timePerFrame = duration.count();
		float timeForRotation = 7.5f;

		float deltaTime = timePerFrame / timeForRotation;

		// A check for to see if full rotation
		if (rotationAmount >= XM_PI*2)
		{
			rotationAmount = 0;
		}		
		
		/*
		if (xDist >= 1.0f)
		{
			xDist = 0.0f;
		}
		*/

		// Adaptivly adding rotation amount for each frame so that it will make a full rotation in a set amount of time'


		rotationAmount += (deltaTime)*XM_2PI;
		//xDist += (deltaTime) * 0.5f;
	}



	for (int i = 0; i < nrOfGBuffers; ++i)
	{
		gBuffer[i]->Release();
		gBufferRtv[i]->Release();
		gBufferSrv[i]->Release();
	}

	delete[] gBuffer;
	delete[] gBufferRtv;
	delete[] gBufferSrv;

	//srv->Release();
	samplerState->Release();
	for (int i = 0; i < nrModels; ++i)
	{
		delete vBuffer[i];
		delete iBuffer[i];
		modelTextures[i]->Release();
		srvModelTextures[i]->Release();
	}

	delete[] vBuffer;
	delete[] iBuffer;
	delete[] modelTextures;
	delete[] srvModelTextures;

	for (int i = 0; i < 6; ++i)
	{
		cubeMapUavArray[i]->Release();
	}
	
	delete[] cubeMapUavArray;
	//cubeMapTexture->Release();
	constantWorldMatrixBuffer->Release();
	constantViewProjMatrixBuffer->Release();
	constantLightBuffer->Release();
	//constantMaterialBuffer->Release(); // exeption unhandeled, constantMaterialBuffer was 0xFFFFFFFFFFFFFFFF
	constantCameraBuffer->Release();
	//cubeMapDSView->Release();
	//cubeMapDSTexture->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	cShader->Release();
	dsView->Release();
	dsTexture->Release();
	dsState->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
