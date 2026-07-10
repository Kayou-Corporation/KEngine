#pragma once

#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
    class AssetLoader
{
public:
    AssetLoader(std::unordered_map<Priority, int> priorityQueuesThreadsCount);
    ~AssetLoader();

private:
    ThreadPool m_pool;
    std::unordered_map<Priority, std::string_view> m_queueNames;
    int m_minimumThreads = 2;


// Deleted constructors & operator
private:
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};
END_NAMESPACE_ASSETMANAGER

