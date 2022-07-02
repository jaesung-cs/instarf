#ifndef INSTARF_GPU_BUFFER_H
#define INSTARF_GPU_BUFFER_H

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace instarf {
namespace gpu {

class Device;

class BufferBase {
public:
  BufferBase() = delete;
  BufferBase(const Device& device, VkBufferUsageFlags usage, VkDeviceSize size, const void* ptr = nullptr);
  virtual ~BufferBase();

  operator VkBuffer() const noexcept { return buffer_; }

private:
  VmaAllocator allocator_ = VK_NULL_HANDLE;

  VkBuffer buffer_ = VK_NULL_HANDLE;
  VmaAllocation allocation_ = VK_NULL_HANDLE;

  VkBuffer stagingBuffer_ = VK_NULL_HANDLE;
  VmaAllocation stagingAllocation_ = VK_NULL_HANDLE;
};

template <typename T>
class Buffer : public BufferBase {
public:
  Buffer() = delete;

  Buffer(const Device& device, VkBufferUsageFlags usage, uint64_t size) : BufferBase(device, usage, sizeof(T) * size), size_(size) {}

  Buffer(const Device& device, VkBufferUsageFlags usage, const std::vector<T>& data) : BufferBase(device, usage, sizeof(T) * data.size(), data.data()), size_(data.size()) {}

  ~Buffer() override = default;

  auto size() const noexcept { return size_; }

private:
  uint64_t size_ = 0;
};

template <typename T, typename Traits>
class BufferWithTraits : public Buffer<T> {
private:
  static constexpr VkBufferUsageFlags usage = Traits::usage;

public:
  BufferWithTraits() = delete;

  BufferWithTraits(const Device& device, uint64_t size) : Buffer(device, usage, size) {}

  BufferWithTraits(const Device& device, const std::vector<T>& data) : Buffer(device, usage, data) {}

  ~BufferWithTraits() override = default;

private:
};

struct VertexBufferTraits {
  static constexpr VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

struct IndexBufferTraits {
  static constexpr VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

template <typename T>
using VertexBuffer = BufferWithTraits<T, VertexBufferTraits>;

using IndexBuffer = BufferWithTraits<uint32_t, IndexBufferTraits>;

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_BUFFER_H
