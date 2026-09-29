#include "Asset.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

Asset::Asset(const AssetType type, const std::string& name, const uint32_t id)
{
    m_type = type;
    m_name = name;
    m_id = id;
}

//Core::KUniquePtr<T> Load()


END_NAMESPACE_ASSETMANAGER
