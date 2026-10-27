#pragma once

#include "AssetManagerModule.hpp"

#include "AssetRegistry.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

class InternalAssetSerializer
{
public:
    KAPI static void WriteAssetHeader(std::ofstream& asset, const AssetHeader& headerInfo);
    KAPI static AssetHeader ReadAssetHeader(std::ifstream& asset);

    template<typename RawData>
    static void WriteAssetRawData(std::ofstream& file, const RawData& data);

    template<typename RawData>
    RawData ReadAssetRawData(std::ifstream& file);


public:
    InternalAssetSerializer() = delete;
    ~InternalAssetSerializer() = delete;

    InternalAssetSerializer(const InternalAssetSerializer&) = delete;
    InternalAssetSerializer& operator=(const InternalAssetSerializer&) = delete;
    InternalAssetSerializer(InternalAssetSerializer&&) = delete;
    InternalAssetSerializer& operator=(InternalAssetSerializer&&) = delete;
};

END_NAMESPACE_ASSETMANAGER
