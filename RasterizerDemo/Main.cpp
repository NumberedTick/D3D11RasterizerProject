#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <chrono>
#include <vector>
#include <DirectXCollision.h>
#include <map>

#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "IndexBufferD3D11.h"
#include "VertexBufferD3D11.h"
#include "CameraD3D11.h"
#include "MeshD3D11.h"
#include "Entity.h"

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView** rtvArr,
			ID3D11DepthStencilView* dsView, ID3D11DepthStencilState* dsState, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
			ID3D11PixelShader* pShader, ID3D11ComputeShader* cShader,ID3D11InputLayout* inputLayout, 
			ID3D11Buffer* tempConstantBuffer, ID3D11Buffer* viewProjBuffers, const unsigned int nrOfGBuffers, MeshD3D11* mesh)
{
	UINT meshStride = mesh->GetVertexSize();
	UINT offset = 0;

	// Set buffers used in the rendering pipeline for the current mesh
	ID3D11Buffer* meshVertexBuffer[] = { mesh->GetVertexBuffer()};
	ID3D11Buffer* meshIndexBuffer[] = { mesh->GetIndexBuffer() };
	ID3D11Buffer* meshMaterialBuffer = mesh->GetMaterialBuffer();

	immediateContext->IASetVertexBuffers(0, 1, meshVertexBuffer, &meshStride, &offset);
	immediateContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->OMSetDepthStencilState(dsState, 0);
	immediateContext->VSSetShader(vShader, nullptr, 0);	
	immediateContext->VSSetConstantBuffers(0, 1, &tempConstantBuffer);
	immediateContext->VSSetConstantBuffers(1, 1, &viewProjBuffers);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &meshMaterialBuffer);
	immediateContext->OMSetRenderTargets(nrOfGBuffers, rtvArr, dsView);

	immediateContext->DrawIndexed(mesh->GetNrOfIndices(), 0, 0);

	immediateContext->VSSetConstantBuffers(0, 0, nullptr);
}


void RenderReflectivObject(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView** rtvArr,
	ID3D11DepthStencilView* dsView, ID3D11DepthStencilState* dsState, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader, ID3D11ComputeShader* cShaderCubeMap, ID3D11UnorderedAccessView**& cubeMapUavArray,ID3D11InputLayout* inputLayout, CameraD3D11** cubeMapCameras,  
	ID3D11Buffer** worldMatrixBuffer, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView** gBufferCubeMapSRV, const unsigned int nrOfGBuffers, 
	ID3D11ShaderResourceView**& srvMeshTextures, std::vector<std::unique_ptr<MeshD3D11>>& meshVector, std::vector<std::unique_ptr<Entity>>& entityVector)
{
	
	ID3D11RenderTargetView* nullRTV = nullptr;

	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->OMSetDepthStencilState(dsState, 0);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);


	UINT offset = 0;
	UINT meshRenderID = -1;


	float clearColour[4] = { 0, 0, 0, 0 };

	for (int i = 0; i < 6; ++i) // Render relevant objects for each of the six sides in the texture cube. 
	{
		// add for loop for each model to render one side at a time
		// add check if model is the cube map itself
		for (int j = 0; j < nrOfGBuffers; ++j)
		{
			immediateContext->ClearRenderTargetView(rtvArr[j], clearColour);
		}

		immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		XMFLOAT4X4 tempFloat4X4 = cubeMapCameras[i]->GetViewProjectionMatrix();
		cubeMapCameras[i]->UpdateInternalConstantBuffer(immediateContext);
		ID3D11Buffer* currentBuffer = cubeMapCameras[i]->GetConstantBuffer();

		for (int k = 0; k < 4; ++k) {
			meshRenderID = entityVector[k].get()->getMeshID();
			if (!entityVector[k].get()->isCubeMap()) {

				// Set buffers used in the rendering pipeline for the current mesh
				UINT meshStride = meshVector[meshRenderID].get()->GetVertexSize();
				ID3D11Buffer* meshVertexBuffer[] = { meshVector[meshRenderID].get()->GetVertexBuffer() };
				ID3D11Buffer* meshIndexBuffer =  meshVector[meshRenderID].get()->GetIndexBuffer();
				ID3D11Buffer* meshMaterialBuffer = meshVector[meshRenderID].get()->GetMaterialBuffer();

				immediateContext->IASetVertexBuffers(0, 1, meshVertexBuffer, &meshStride, &offset);
				immediateContext->IASetIndexBuffer(meshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				immediateContext->VSSetConstantBuffers(0, 1, &worldMatrixBuffer[k]);
				immediateContext->PSSetShaderResources(0, 1, &srvMeshTextures[k]);
				immediateContext->PSSetConstantBuffers(0, 1, &meshMaterialBuffer);
				immediateContext->VSSetConstantBuffers(1, 1, &currentBuffer);
				immediateContext->OMSetRenderTargets(nrOfGBuffers, rtvArr, dsView);
				immediateContext->DrawIndexed(meshVector[meshRenderID].get()->GetNrOfIndices(), 0, 0);
			}
		}



		immediateContext->OMSetRenderTargets(1, &nullRTV, nullptr);
		// go out of for loop and draw again. 
		immediateContext->CSSetShader(cShaderCubeMap, nullptr, 0);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &cubeMapUavArray[i], nullptr);
		immediateContext->CSSetShaderResources(0, nrOfGBuffers, gBufferCubeMapSRV);
		immediateContext->Dispatch(1024 / 8, 1024 / 8, 1);	
		immediateContext->VSSetConstantBuffers(0, 0, nullptr);
		// clear gbuffers for next side of the cube. 
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
	bool DynamicCubeMapsEnabled = true; // Might remove and replace with Dear ImGui for dynamic toggle

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
	ID3D11PixelShader* pShaderCubeMap;
	ID3D11ComputeShader* cShader;
	ID3D11ComputeShader* cShaderCubeMap;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantWorldMatrixBuffer;
	ID3D11Buffer* constantViewProjMatrixBuffer;
	ID3D11Buffer* constantLightBuffer;
	ID3D11Buffer* constantCameraBuffer;
	ConstantBufferD3D11 cameraPositionBuffer;

	ID3D11SamplerState* samplerState;
	ID3D11UnorderedAccessView* uav;
	ID3D11UnorderedAccessView* uavTextureCube;

	CameraD3D11 mainCamera;

	// creation of the needed things for the cubemap
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
	
	//MeshData testMesh;

	// Loads meshes into the scene
	std::vector<std::string> meshNames;

	meshNames = {
		"roomHoles.obj",
		"untitled.obj",
		"torus.obj",
		"smoothSphere.obj"
	};

	UINT nrOfMeshes = static_cast<UINT>(meshNames.size());

	std::vector<std::unique_ptr<MeshD3D11>> meshVector; // Use unique_ptr to manage memory automatically

	std::map<std::string, UINT> meshIDMap; // Map to store mesh names and their IDs

	// Loads textures

	std::vector<std::string> textureNames;

	textureNames = {
		"torus.png",
		"texture2.png",
		"texture.jpg",
	};


	std::vector<Material> materialVector; // Use unique_ptr to manage memory automatically

	std::vector<std::unique_ptr<ConstantBufferD3D11>> materialBufferVector; // Use unique_ptr to manage memory automatically

	std::string missingTexture = "missing.jpg";
	// Creates VertexBuffers for each model loaded (currently doing it manually)

	std::vector<std::unique_ptr<VertexBufferD3D11>> uniqueVBuffer;


	std::vector<std::unique_ptr<Entity>> entityVector; // Use unique_ptr to manage memory automatically

	for (int i = 0; i < nrOfMeshes; ++i)
	{
		meshVector.emplace_back(std::make_unique<MeshD3D11>()); // Store in unique_ptr to manage memory automatically

		materialVector.emplace_back(); // Store in unique_ptr to manage memory automatically
		materialBufferVector.emplace_back(std::make_unique<ConstantBufferD3D11>()); // Store in unique_ptr to manage memory automatically

		// Fill entity vector with the same number of meshes to just start Rendering with entites REMOVE LATER
		entityVector.emplace_back(std::make_unique<Entity>());

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

	// Creation of Cube Mapping gBuffers for deffered rendering
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

		if (DynamicCubeMapsEnabled)
		{

			if (!CreateGBuffer(device, gTextureBuffer[i], gBufferTextureRtv[i], gBufferCubeMapSRV[i], 256, 256))
			{
				std::cerr << "Error creating G-Buffer for Texture Cube!" << std::endl;
				return false;
			}

			cubeMapRtvGBufferArr[i] = gBufferTextureRtv[i];
			cubeMapSrvGBufferArr[i] = gBufferCubeMapSRV[i];
		}
	}


	if (!SetupPipeline(device, vShader, pShader, pShaderCubeMap, cShader, cShaderCubeMap, inputLayout, 
		constantWorldMatrixBuffer, constantViewProjMatrixBuffer, constantLightBuffer,
		constantCameraBuffer, immediateContext, cubeMapTexture, cubeMapUavArray,
		cubeMapSrv, cubeMapCameras,cubeMapViewport, cubeMapDSTexture,cubeMapDSView,cubeMapDSState,
		samplerState, meshNames, WIDTH, HEIGHT, uavTextureCube, mainCamera, cameraPositionBuffer,
		uniqueVBuffer, meshVector, meshIDMap))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}
	

	// Creation of the needed textures for each model
	ID3D11Texture2D** meshTextures = new ID3D11Texture2D* [nrOfMeshes];
	ID3D11ShaderResourceView** srvMeshTextures = new ID3D11ShaderResourceView* [nrOfMeshes];
	for (int i = 0; i < nrOfMeshes; ++i)
	{
		if (i < textureNames.size()) 
		{
			if (!Create2DTexture(device, meshTextures[i], textureNames[i]))
			{
				std::cerr << "Failed to create 2DTexture for model!" << std::endl;
				return -1;
			}

			if (!CreateSRV(device, meshTextures[i], srvMeshTextures[i]))
			{
				std::cerr << "Failed to create SRV for model texture!" << std::endl;
				return -1;
			}
		}
		else // only used to create missing texture if there are more models than textures,
		{
			if (!Create2DTexture(device, meshTextures[i], missingTexture))
			{
				std::cerr << "Failed to create 2DTexture for missing texture!" << std::endl;
				return -1;
			}

			if (!CreateSRV(device, meshTextures[i], srvMeshTextures[i]))
			{
				std::cerr << "Failed to create SRV for model texture!" << std::endl;
				return -1;
			}
		}
		
	}

	XMFLOAT3 entityPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 entityPos2 = { 0.0f, 0.0f, -2.0f };
	XMFLOAT3 entityRot = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 entityScale = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 entityScale2 = { 0.5f, 0.5f, 0.5f };

	XMFLOAT3 cubeMapPos = {0.0f, 0.0f, -0.5f };
	XMFLOAT3 cubeMapRot = { 0.0f, XM_PIDIV2, 0.0f };

	entityVector[0].get()->Initialize(device, entityPos, entityRot, entityScale, "roomHoles.obj", meshIDMap, false);
	entityVector[1].get()->Initialize(device, entityPos2, entityRot, entityScale, "torus.obj", meshIDMap, false);
	entityVector[2].get()->Initialize(device, entityPos2, entityRot, entityScale2, "torus.obj", meshIDMap, false);
	entityVector[3].get()->Initialize(device, cubeMapPos, cubeMapRot, entityScale, "smoothSphere.obj", meshIDMap, true);

	// create entities and assign index buffer ID, vertex buffer ID, Texture ID, modle name, texture name, 

	MSG msg = { };

	XMFLOAT4X4 float4x4Array[3];

	int cubeMapIndex = -1;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	float rotationAmount = 0.0f;

	float xDist = 2.0f;

	float yDist = 0.0f;

	float zDist = 0.5f;

	float clearColour[4] = { 0, 0, 0, 0 };

	XMMATRIX cubemapMatrix;

	// TEMP VALAUES USED UNTIL I SWITCH MAIN CAMERA TO A CAMERA OBJECT

	XMMATRIX baselineWorldMatrix = CreateWorldMatrix(XM_PIDIV2, 0.0f, 0.0f, -0.5f);
	XMStoreFloat4x4(&float4x4Array[1], baselineWorldMatrix);

	XMMATRIX newWorldMatrix = CreateWorldMatrix(XM_PIDIV2, -xDist, 0.0f, -0.5f);
	XMStoreFloat4x4(&float4x4Array[2], newWorldMatrix);

	XMVECTOR eyePosition = { 1.0f, 0.0f, -3.5f };
	XMVECTOR viewVecotr = { -0.6f, 0.0f, 1.0f };
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

	ID3D11Buffer* tempBufferArray[4];
	tempBufferArray[0] = tempBuffer;
	tempBufferArray[1] = constantWorldMatrixBuffer;
	tempBufferArray[2] = tempBuffer2;
	tempBufferArray[3] = tempBuffer;

	ID3D11Buffer* bufferArray[2] = { constantLightBuffer, constantCameraBuffer};

	ID3D11Buffer* currentBuffer;
	XMFLOAT3 eyePositionFloat3;
	XMStoreFloat3(&eyePositionFloat3, eyePosition);

	XMFLOAT3 position = mainCamera.GetPosition();

	ID3D11Buffer* bufferArray2[3] = { constantLightBuffer, cameraPositionBuffer.GetBuffer()};

	//bool willBeRendred = false;

	//std::vector<int> renderIDs;
	/*	
	BoundingBox boundingBox[4];
	
	boundingBox[0].CreateFromPoints(boundingBox[0], 2, boundingBoxCorners, sizeof(SimpleVertex));
	boundingBox[0].GetCorners(boundingBoxCorners);


	XMFLOAT3 boundingBoxCorners[] = { XMFLOAT3(1, 1, 1), XMFLOAT3(-1, -1, -1) };
	boundingBox.CreateFromPoints(boundingBox, 2, boundingBoxCorners, sizeof(SimpleVertex));
	
	*/
	

	//Entity entinty;

	XMFLOAT4X4 cameraViewProjMatrixFloat4x4 = mainCamera.GetViewProjectionMatrix();

	XMMATRIX cameraViewProjMatrix = XMLoadFloat4x4(&cameraViewProjMatrixFloat4x4);

	BoundingFrustum boundingFrustum;
	boundingFrustum.CreateFromMatrix(boundingFrustum, cameraViewProjMatrix);
	//boundingFrustum

	//XMFLOAT3 boundingBoxCorners[8];

	// Main loop
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Start time for the time to render a frame
		auto startTime = std::chrono::high_resolution_clock::now();


		// Check to see what entities will be rendered (going to add frustum culling and quadtree to this later)
		for (int i = 0; i < entityVector.size(); ++i)
		{
			if (entityVector[i].get()->isCubeMap()) 
			{
				cubeMapIndex = i;
			}

		}


		// creation of the new world matrix for the rotation
		newWorldMatrix = CreateWorldMatrix(rotationAmount, xDist, yDist, zDist);
		XMStoreFloat4x4(&float4x4Array[0], newWorldMatrix);

		// Mapping the new world matrix to the vertex shader
		immediateContext->Map(constantWorldMatrixBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, &float4x4Array, sizeof(XMFLOAT4X4));
		immediateContext->Unmap(constantWorldMatrixBuffer, 0);



		mainCamera.UpdateInternalConstantBuffer(immediateContext);
		cameraViewProjMatrixFloat4x4 = mainCamera.GetViewProjectionMatrix();
		cameraViewProjMatrix = XMLoadFloat4x4(&cameraViewProjMatrixFloat4x4);
		boundingFrustum.Transform(boundingFrustum, cameraViewProjMatrix);

		

		//boundingFrustum.GetCorners(boundingBoxCorners);

		//boundingBoxCorners;
		/*
		
		for (int id = 0; id < nrEntites; id++) 
		{
			// check which entities are suposed to render

			if (willBeRendred)
			{
				renderIDs.push_back(id);
			}
		}
		*/


		// Rendering part

		// Rendering the dynamic cube map
		if (DynamicCubeMapsEnabled)
		{
			RenderReflectivObject(immediateContext, cubeMapRtvGBufferArr, cubeMapDSView, cubeMapDSState,
					cubeMapViewport, vShader, pShader, cShaderCubeMap, cubeMapUavArray, inputLayout, cubeMapCameras, 
				tempBufferArray, uavTextureCube, gBufferCubeMapSRV, nrOfGBuffers, srvMeshTextures, meshVector, entityVector);
		}

		// Cleararing from last frame of main rendering
		for (int i = 0; i < nrOfGBuffers; ++i)
		{
			immediateContext->ClearRenderTargetView(rtvArr[i], clearColour);
		}
		immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		currentBuffer = mainCamera.GetConstantBuffer();
		position = mainCamera.GetPosition();
		cameraPositionBuffer.UpdateBuffer(immediateContext, &position);
		bufferArray2[1] = cameraPositionBuffer.GetBuffer();
		bufferArray2[2] = currentBuffer;

		// Gemoetry pass
		for (int i = 0; i < nrOfMeshes; ++i)
		{
			if (i != cubeMapIndex) 
			{
				immediateContext->PSSetShaderResources(0, 1, &srvMeshTextures[i]);
				//immediateContext->VSSetConstantBuffers(1, 1, &currentBuffer);
				Render(immediateContext, rtvArr, dsView, dsState,
					viewport, vShader, pShader, cShader, inputLayout,
					 tempBufferArray[i], currentBuffer, nrOfGBuffers, meshVector[entityVector[i].get()->getMeshID()].get());
			}
			
		}
		// Unbinding GBuffer RTVs
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		immediateContext->OMSetRenderTargets(1, nullRTV, nullptr);

		// Shading pass
		immediateContext->CSSetShader(cShader, nullptr, 0);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		immediateContext->CSSetShaderResources(0, nrOfGBuffers, srvArr);
		immediateContext->CSSetConstantBuffers(0, 3, bufferArray2);
		immediateContext->Dispatch(WIDTH / 8, HEIGHT / 8, 1);


		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

		immediateContext->CSSetShaderResources(0, 1, nullSRV);
		immediateContext->CSSetShader(nullptr, nullptr, NULL);

		// Forward rendering off cubemap
		if (DynamicCubeMapsEnabled and cubeMapIndex >= 0) {
			immediateContext->PSSetShaderResources(0, 1, &cubeMapSrv);
			immediateContext->PSSetConstantBuffers(1, 1, &bufferArray2[1]);
			Render(immediateContext, rtvArr, dsView, dsState,
				viewport, vShader, pShaderCubeMap, cShaderCubeMap, inputLayout,
				tempBufferArray[3], currentBuffer, nrOfGBuffers, meshVector[entityVector[cubeMapIndex].get()->getMeshID()].get());
			immediateContext->PSSetConstantBuffers(1, 0, nullptr);
		}

		// Unbinding GBuffer RTVs
		//ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		immediateContext->OMSetRenderTargets(1, nullRTV, nullptr);

		immediateContext->CSSetShader(cShader, nullptr, 0);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		immediateContext->CSSetShaderResources(0, nrOfGBuffers, srvArr);
		immediateContext->CSSetConstantBuffers(0, 3, bufferArray2);
		immediateContext->Dispatch(WIDTH / 8, HEIGHT / 8, 1);

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

		
		// Check for key presses
		if (GetKeyState('S') & 0x8000)
		{
			mainCamera.MoveForward(-0.1);
		}
		if (GetKeyState('W') & 0x8000)
		{
			mainCamera.MoveForward(0.1);
		}
		if (GetKeyState('A') & 0x8000)
		{
			mainCamera.MoveRight(-0.05);
		}
		if (GetKeyState('D') & 0x8000)
		{
			mainCamera.MoveRight(0.05);
		}
		if (GetKeyState(VK_SHIFT) & 0x8000) 
		{
			mainCamera.MoveUp(-0.05);
		}
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			mainCamera.MoveUp(0.05);
		}
		if (GetKeyState(VK_UP) & 0x8000)
		{
			mainCamera.RotateRight(-XM_2PI / 720);
		}
		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			mainCamera.RotateRight(XM_2PI / 720);
		}
		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			mainCamera.RotateUp(XM_2PI / 720);
		}
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			mainCamera.RotateUp(-XM_2PI / 720);
		}


		// Adaptivly adding rotation amount for each frame so that it will make a full rotation in a set amount of time'
		rotationAmount += (deltaTime)*XM_2PI;
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

	samplerState->Release();
	for (int i = 0; i < nrOfMeshes; ++i)
	{
		meshTextures[i]->Release();
		srvMeshTextures[i]->Release();
	}

	delete[] meshTextures;
	delete[] srvMeshTextures;

	for (int i = 0; i < 6; ++i)
	{
		cubeMapUavArray[i]->Release();
	}
	
	delete[] cubeMapUavArray;
	//cubeMapTexture->Release();
	constantWorldMatrixBuffer->Release();
	//constantViewProjMatrixBuffer->Release();
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
