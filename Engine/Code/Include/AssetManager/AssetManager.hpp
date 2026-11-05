#pragma once

#include "AssetManagerModule.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

enum class AssetType
{
    StaticMesh = 0,
    Texture2D = 1,

    DefaultGuess = 2,
    COUNT = 3
};



END_NAMESPACE_ASSETMANAGER
