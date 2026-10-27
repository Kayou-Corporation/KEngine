#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"
#include "MeshBase.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

struct RawData
{
    // Empty, only useful for heritage.
};

struct RawStaticMeshData : public RawData
{
    std::vector<CoreObject::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<CoreObject::SubMesh> submeshes;
};

struct RawTextureData : public RawData
{
    std::vector<uint8_t> pixels;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

class ExternalAssetLoader
{
public:
    template<typename RawData>
    KAPI static RawData LoadAssetFromSource(const std::string_view& path);

    KAPI static AssetType GuessAssetTypeFromSource(const std::string_view& path);

public:
    ExternalAssetLoader() = delete;
    ~ExternalAssetLoader() = delete;

    ExternalAssetLoader(const ExternalAssetLoader&) = delete;
    ExternalAssetLoader& operator=(const ExternalAssetLoader&) = delete;
    ExternalAssetLoader(ExternalAssetLoader&&) = delete;
    ExternalAssetLoader& operator=(ExternalAssetLoader&&) = delete;
};
END_NAMESPACE_ASSETMANAGER

