#include  "AssetLoader.hpp"

//#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_ASSETMANAGER
void AssetLoader::CreateAsset(const std::string_view &assetPath)
{
    AssetType type = GuessAssetTypeFromSource(assetPath);
    (void)type;

}

AssetType AssetLoader::GuessAssetTypeFromSource(const std::string_view &path)
{
    const std::string_view::size_type dotPos = path.find_last_of('.');
    if (dotPos == std::string_view::npos || dotPos == path.length() - 1)
    {
        spdlog::error("AssetLoader::GuessAssetTypeFromSource: Invalid file path or missing extension: {}", path);
        return AssetType::COUNT;
    }

    std::string filExtension(path.substr(dotPos + 1));

    std::transform(filExtension.begin(), filExtension.end(), filExtension.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (filExtension == "obj" || filExtension == "fbx" || filExtension == "gltf")
    {
        return AssetType::StaticMesh;
    }
    else if (filExtension == "png" || filExtension == "jpg" || filExtension == "jpeg" || filExtension == "bmp")
    {
        return AssetType::Texture;
    }
    else
    {
        spdlog::error("AssetLoader::GuessAssetTypeFromSource: Unsupported file extension: {}", filExtension);
        return AssetType::COUNT;
    }

    return AssetType::COUNT;
}

END_NAMESPACE_ASSETMANAGER
