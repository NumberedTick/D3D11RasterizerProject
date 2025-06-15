#include "EntityCreation.h"
#include "Entity.h"

void InitializeEntities(ID3D11Device* device, std::vector<std::unique_ptr<Entity>>& entityVector, const std::map<std::string, UINT>& meshIDMap, const std::map<std::string, UINT>& textureIDMap)
{
	XMFLOAT3 entityPos[] = {
		{ 0.0f, 0.0f, -0.5f },
		{ 0.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, -2.0f },
		{ 1.0f, 0.0f, -0.5f },
		{ 0.0f, 3.5f, 3.5f },
		{ 0.0f, 0.0f, -3.5f },
		{ 20.0f, 0.0f, -3.5f }
	};

	XMFLOAT3 entityRot[] = {
		{0.0f, 0.0f, 0.0f},
	};

	XMFLOAT3 entityScale[] = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 1.5f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.5f, 1.5f, 1.5f },
		{ 0.5f, 0.5f, 0.5f }
	};

	entityVector[0].get()->Initialize(device, entityPos[0], entityRot[0], entityScale[0], "roomHoles.obj", meshIDMap, textureIDMap, false, "texture.jpg");
	entityVector[1].get()->Initialize(device, entityPos[1], entityRot[0], entityScale[1], "torus.obj", meshIDMap, textureIDMap, false, "torus.png");
	entityVector[2].get()->Initialize(device, entityPos[2], entityRot[0], entityScale[2], "torus.obj", meshIDMap, textureIDMap, false, "torus.png");
	entityVector[3].get()->Initialize(device, entityPos[3], entityRot[0], entityScale[3], "smoothSphere.obj", meshIDMap, textureIDMap, true, "torus.png");
	entityVector[4].get()->Initialize(device, entityPos[4], entityRot[0], entityScale[4], "untitled.obj", meshIDMap, textureIDMap, false, "texture2.png");
	entityVector[5].get()->Initialize(device, entityPos[5], entityRot[0], entityScale[5], "untitled.obj", meshIDMap, textureIDMap, false, "");
	entityVector[6].get()->Initialize(device, entityPos[6], entityRot[0], entityScale[0], "torus.obj", meshIDMap, textureIDMap, false, "texture2.png");
	entityVector[7].get()->Initialize(device, entityPos[6], entityRot[0], entityScale[1], "roomHoles.obj", meshIDMap, textureIDMap, false, "");
	entityVector[8].get()->Initialize(device, entityPos[5], entityRot[0], entityScale[5], "untitled.obj", meshIDMap, textureIDMap, false, "");

}