#pragma once

#include "AssetManagerModule.hpp"
#include  "StackAllocator.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

class AssetRegistry
{
public:


// List of asset by type with id ?

private:
    Memory::StackAllocator m_allocator;

// Deleted constructors & operator
private:
    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    AssetRegistry(AssetRegistry&&) = delete;
    AssetRegistry& operator=(AssetRegistry&&) = delete;
};
END_NAMESPACE_ASSETMANAGER


