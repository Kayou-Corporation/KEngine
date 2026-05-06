#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Buffer;
class Image;

class DescriptorSet : public virtual Core::IResource
{
public:
	virtual ~DescriptorSet() = default;

	KENGINE_API virtual void SetBuffer(std::string name, DescriptorType type, Core::RefCountPtr<Buffer> buffer) = 0;
	KENGINE_API virtual void SetBuffer(uint32_t index, DescriptorType type, Core::RefCountPtr<Buffer> buffer) = 0;

	KENGINE_API virtual void SetImage(std::string name, DescriptorType type, Core::RefCountPtr<Image> image) = 0;
	KENGINE_API virtual void SetImage(uint32_t index, DescriptorType type, Core::RefCountPtr<Image> image) = 0;
};

END_NAMESPACE_RHI