#pragma once

#include "AssetManagerModule.hpp"

#include "ThreadManager.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

class AssetLoader
{
public:
    AssetLoader();
    ~AssetLoader();

private:
    ThreadManager m_threadManager;
};
END_NAMESPACE_ASSETMANAGER

