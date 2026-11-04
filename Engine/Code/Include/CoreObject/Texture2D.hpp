#pragma once

#include "CoreObjectModule.hpp"

#include "Asset.hpp"
#include "RHI/API/Image.hpp"

BEGIN_NAMESPACE_COREOBJECT

class Texture2D : public AssetManager::Asset
{
public:
    KAPI Texture2D(const AssetManager::AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize);
    KAPI virtual ~Texture2D();

    // Base utility
    KAPI uint32_t GetWidth() const {return m_width;}
    KAPI uint32_t GetHeight() const {return m_height;}
    KAPI uint32_t GetChannelsCount() const {return m_channelsCount;}

    KAPI Core::ImageHandle GetImage() const { return m_image; }

protected:
    KAPI bool Load(std::ifstream& file) override;
    KAPI void UnLoad() override;

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channelsCount = 0;

    Core::ImageHandle m_image;
};

END_NAMESPACE_COREOBJECT