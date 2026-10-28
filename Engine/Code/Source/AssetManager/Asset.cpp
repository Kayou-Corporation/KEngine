#include "Asset.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

Asset::Asset(const AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize)
{
    m_type = type;
    m_name = name;
    m_id = id;
    m_sourceHash = sourceHash;
    m_sourceSize = sourceSize;
}



END_NAMESPACE_ASSETMANAGER
