#pragma once

#include  "Utils/Memory.hpp"
#include "Export.hpp"
#include "AssetManagerModule.hpp"

#include "AssetManager.hpp"
//#include "Engine.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

/*
 * Type d'asset : TypeID
 * Name
 * Identification : ID
 * Metadata depending on type ?
 * Copy of original data (std::vector<DataType> depending on Type of mesh)
 *
 */
class Asset : public virtual Core::IResource
{
    friend class AssetRegistry;

public:
    KAPI Asset(const AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize);
    KAPI virtual ~Asset() = default;

    KAPI AssetType GetType() const { return m_type; }
    KAPI std::string GetName() const { return m_name; }
    KAPI uint32_t GetID() const { return m_id; }
    KAPI bool IsLoaded() const {return isLoaded;}

    void OnUnload() override
    {
        if (isLoaded)
        {
            UnLoad();
            isLoaded = false;
        }
    }


protected:
    KAPI virtual bool Load(std::ifstream& file) { (void) file; return true; }
    KAPI virtual void UnLoad() {}

    AssetType m_type;
    std::string m_name;
    uint32_t m_id;

    //std::string_view AssetPath;
    bool isLoaded = false;
    uint32_t m_sourceHash;
    uint32_t m_sourceSize;

    // Deleted constructors & operator
private:
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;
    Asset(Asset&&) = delete;
    Asset& operator=(Asset&&) = delete;
};

END_NAMESPACE_ASSETMANAGER

