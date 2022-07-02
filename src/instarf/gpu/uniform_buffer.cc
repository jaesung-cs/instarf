#include <instarf/gpu/uniform_buffer.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {
namespace {
VkDeviceSize align(VkDeviceSize offset, VkDeviceSize alignment) { return (offset + alignment - 1) & ~(alignment - 1); }
}  // namespace

UniformBufferBase::UniformBufferBase(const Device& device, uint32_t elementSize, uint32_t size) : allocator_(device.allocator()) {
  auto physicalDevice = device.physicalDevice();

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  const auto hardwareAlignment = properties.limits.minUniformBufferOffsetAlignment;
  alignment_ = align(elementSize, hardwareAlignment);

  VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufferInfo.size = alignment_ * size;
  bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  VmaAllocationCreateInfo allocationCreateInfo = {};
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

  VmaAllocationInfo allocationInfo;
  vmaCreateBuffer(allocator_, &bufferInfo, &allocationCreateInfo, &buffer_, &allocation_, &allocationInfo);

  map_ = reinterpret_cast<uint8_t*>(allocationInfo.pMappedData);
}

UniformBufferBase::~UniformBufferBase() { vmaDestroyBuffer(allocator_, buffer_, allocation_); }

}  // namespace gpu
}  // namespace instarf
