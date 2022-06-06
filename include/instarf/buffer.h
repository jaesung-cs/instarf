#ifndef INSTARF_BUFFER_H
#define INSTARF_BUFFER_H

#include <memory>

#include <vulkan/vulkan.h>

#include <instarf/engine.h>

namespace instarf {

class BufferBase {
public:
  BufferBase() = default;
  BufferBase(Engine engine, VkBufferUsageFlags usage, VkDeviceSize size,
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

  Buffer(Engine engine, VkBufferUsageFlags usage, uint64_t size)
      : BufferBase(engine, usage, sizeof(T) * size), size_(size) {}

  Buffer(Engine engine, VkBufferUsageFlags usage, const std::vector<T>& data)
      : BufferBase(engine, usage, sizeof(T) * data.size(), data.data()),
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

  BufferWithTraits(Engine engine, uint64_t size)
      : Buffer(engine, usage, size) {}

  BufferWithTraits(Engine engine, const std::vector<T>& data)
      : Buffer(engine, usage, data) {}

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

}  // namespace instarf

#endif  // INSTARF_BUFFER_H
