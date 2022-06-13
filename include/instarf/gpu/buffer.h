#ifndef INSTARF_GPU_BUFFER_H
#define INSTARF_GPU_BUFFER_H

#include <memory>

#include <vulkan/vulkan.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class BufferBase {
public:
  BufferBase() = default;
  BufferBase(Device device, VkBufferUsageFlags usage, VkDeviceSize size,
             const void* ptr = nullptr);
  virtual ~BufferBase() = default;

  operator VkBuffer() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

template <typename T>
class Buffer : public BufferBase {
public:
  Buffer() = default;

  Buffer(Device device, VkBufferUsageFlags usage, uint64_t size)
      : BufferBase(device, usage, sizeof(T) * size), size_(size) {}

  Buffer(Device device, VkBufferUsageFlags usage, const std::vector<T>& data)
      : BufferBase(device, usage, sizeof(T) * data.size(), data.data()),
        size_(data.size()) {}

  ~Buffer() override = default;

  auto size() const noexcept { return size_; }

private:
  uint64_t size_;
};

template <typename T, typename Traits>
class BufferWithTraits : public Buffer<T> {
private:
  static constexpr VkBufferUsageFlags usage = Traits::usage;

public:
  BufferWithTraits() = default;

  BufferWithTraits(Device device, uint64_t size)
      : Buffer(device, usage, size) {}

  BufferWithTraits(Device device, const std::vector<T>& data)
      : Buffer(device, usage, data) {}

  ~BufferWithTraits() override = default;

private:
};

struct VertexBufferTraits {
  static constexpr VkBufferUsageFlags usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

struct IndexBufferTraits {
  static constexpr VkBufferUsageFlags usage =
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

template <typename T>
using VertexBuffer = BufferWithTraits<T, VertexBufferTraits>;

using IndexBuffer = BufferWithTraits<uint32_t, IndexBufferTraits>;

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_BUFFER_H
