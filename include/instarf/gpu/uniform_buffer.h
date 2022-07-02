#ifndef INSTARF_GPU_UNIFORM_BUFFER_H
#define INSTARF_GPU_UNIFORM_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace instarf {
namespace gpu {

class Device;

class UniformBufferBase {
public:
  UniformBufferBase() = delete;
  UniformBufferBase(const Device& device, uint32_t elementSize, uint32_t size);
  virtual ~UniformBufferBase();

  operator VkBuffer() const noexcept { return buffer_; }

  uint32_t alignment() const noexcept { return alignment_; }
  auto offset(uint32_t index) const noexcept { return alignment_ * index; }

protected:
  uint8_t* ptr() noexcept { return map_; }
  const uint8_t* ptr() const noexcept { return map_; }

private:
  VmaAllocator allocator_ = VK_NULL_HANDLE;

  VkBuffer buffer_ = VK_NULL_HANDLE;
  VmaAllocation allocation_ = VK_NULL_HANDLE;
  uint32_t alignment_ = 0;
  uint8_t* map_ = nullptr;
};

template <typename T>
class UniformBuffer : public UniformBufferBase {
public:
  UniformBuffer() = delete;

  UniformBuffer(const Device& device, uint32_t size) : UniformBufferBase(device, sizeof(T), size) {}

  ~UniformBuffer() override = default;

  T& operator[](uint32_t index) { return *reinterpret_cast<T*>(ptr() + offset(index)); }

  const T& operator[](uint32_t index) const { return *reinterpret_cast<const T*>(ptr() + offset(index)); }

private:
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_UNIFORM_BUFFER_H
