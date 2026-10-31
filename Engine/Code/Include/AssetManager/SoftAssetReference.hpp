#pragma once

#include "AssetManagerModule.hpp"

#include "AssetRegistry.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

template<typename T>
class SoftAssetRef
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
    Core::KSharedPtr<T> Get() const
    {
        if (!IsValid())
        {
            spdlog::error("SoftAssetRef::Get: Invalid SoftAssetRef.");
            return nullptr;
        }

        Core::KSharedPtr<T> shared = m_registry->GetAsset<T>(m_id);

        if (shared == nullptr)
        {
            spdlog::error("SoftAssetRef::Get: Asset with ID {} not found.", m_id);
            return nullptr;
        }

        return shared;
    }


    // TODO : Add load async

private:
    uint32_t m_id;
    AssetRegistry* m_registry;

    SoftAssetRef(uint32_t id, AssetRegistry* registry) : m_id(id), m_registry(registry) {};
};
END_NAMESPACE_ASSETMANAGER
