#pragma once

#include "AssetManagerModule.hpp"


BEGIN_NAMESPACE_ASSETMANAGER

class AssetRegistry;

template<typename T>
class KSoftAssetRef
{
    friend class AssetRegistry;
    friend class Asset;

public:
    uint32_t GetID() const {return m_id;}

    bool IsValid() const
    {
        return m_id != static_cast<uint32_t>(-1) && m_registry != nullptr;
    }

    // Automaticely load if unloaded
    Core::KSharedPtr<T> Get() const;

    // TODO : Add load async

private:
    uint32_t m_id;
    AssetRegistry* m_registry;

    KSoftAssetRef(uint32_t id, AssetRegistry* registry) : m_id(id), m_registry(registry) {};
};
END_NAMESPACE_ASSETMANAGER
