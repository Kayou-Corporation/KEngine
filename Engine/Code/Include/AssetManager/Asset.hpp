#pragma once

#include  "Utils/Memory.hpp"
#include "Export.hpp"
#include "AssetManagerModule.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

enum class AssetType
{
    StaticMesh = 0,
    Texture = 1,

    COUNT = 2
};

/*
 * Type d'asset : TypeID
 * Name
 * Identification : ID
 * Metadata depending on type ?
 * Copy of original data (std::vector<DataType> depending on Type of mesh)
 *
 */
class Asset : Core::IResource
{
public:
    KAPI Asset(const AssetType type, const std::string& name, const uint32_t id);
    KAPI ~Asset() = default;

    KAPI AssetType GetType() const { return m_type; }
    KAPI std::string GetName() const { return m_name; }
    KAPI uint32_t GetID() const { return m_id; }

protected:
    AssetType m_type;
    std::string m_name;
    uint32_t m_id;


// Deleted constructors & operator
private:
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;
    Asset(Asset&&) = delete;
    Asset& operator=(Asset&&) = delete;
};

template<typename T>
class SoftAssetRef
{
public:
    Core::RefCountPtr<T> Load();
    Core::RefCountPtr<T> LoadAsync();

    //KAPI AssetType GetType() const { return ; }
    //KAPI std::string GetName() const { return m_name; }
    KAPI uint32_t GetID() const { return m_id; }

private:
    uint32_t m_id;

private:
    SoftAssetRef() = delete;
};

END_NAMESPACE_ASSETMANAGER

