#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <DirectXMath.h>
using namespace DirectX;

#include "Entity.h"


// Function used to clean up the main file a bit.
void InitializeEntities(ID3D11Device* device, std::vector<std::unique_ptr<Entity>>& entityVector, const std::map<std::string, UINT>& meshIDMap,	const std::map<std::string, UINT>& textureIDMap);