#include <instarf/uniform_buffer.h>

#include "vk_mem_alloc.h"

#include <instarf/engine.h>

namespace instarf {
namespace {
VkDeviceSize align(VkDeviceSize offset, VkDeviceSize alignment) {
  return (offset + alignment - 1) & ~(alignment - 1);
}
}  // namespace

class UniformBufferBase::Impl {
public:
  Impl() = delete;

  Impl(Engine engine, uint32_t elementSize, uint32_t size) : engine_(engine) {
    auto physicalDevice = engine_.physicalDevice();
    auto allocator = engine_.allocator();

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    const auto hardwareAlignment =
        properties.limits.minUniformBufferOffsetAlignment;
    alignment_ = align(elementSize, hardwareAlignment);

    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = alignment_ * size;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                 VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationInfo allocationInfo;
    vmaCreateBuffer(allocator, &bufferInfo, &allocationCreateInfo, &buffer_,
                    &allocation_, &allocationInfo);

    map_ = reinterpret_cast<uint8_t*>(allocationInfo.pMappedData);
  }

  ~Impl() {
    auto allocator = engine_.allocator();
    vmaDestroyBuffer(allocator, buffer_, allocation_);
  }

  operator VkBuffer() const noexcept { return buffer_; }

  uint32_t alignment() const noexcept { return alignment_; }

  uint8_t* ptr() noexcept { return map_; }
  const uint8_t* ptr() const noexcept { return map_; }

private:
  Engine engine_;

  VkBuffer buffer_;
  VmaAllocation allocation_;
  uint32_t alignment_;
  uint8_t* map_;
};

UniformBufferBase::UniformBufferBase(Engine engine, uint32_t elementSize,
                                     uint32_t size)
    : impl_(std::make_shared<Impl>(engine, elementSize, size)) {}

UniformBufferBase::operator VkBuffer() const { return *impl_; }

uint32_t UniformBufferBase::alignment() const { return impl_->alignment(); }

uint8_t* UniformBufferBase::ptr() { return impl_->ptr(); }
const uint8_t* UniformBufferBase::ptr() const { return impl_->ptr(); }

}  // namespace instarf
