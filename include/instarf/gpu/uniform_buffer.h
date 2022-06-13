#ifndef INSTARF_GPU_UNIFORM_BUFFER_H
#define INSTARF_GPU_UNIFORM_BUFFER_H

#include <memory>

#include <vulkan/vulkan.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class UniformBufferBase {
public:
  UniformBufferBase() = default;
  UniformBufferBase(Device device, uint32_t elementSize, uint32_t size);
  virtual ~UniformBufferBase() = default;

  operator VkBuffer() const;

  uint32_t alignment() const;
  auto offset(uint32_t index) const { return alignment() * index; }

protected:
  uint8_t* ptr();
  const uint8_t* ptr() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

template <typename T>
class UniformBuffer : public UniformBufferBase {
public:
  UniformBuffer() = default;

  UniformBuffer(Device device, uint32_t size)
      : UniformBufferBase(device, sizeof(T), size) {}

  ~UniformBuffer() override = default;

  T& operator[](uint32_t index) {
    return *reinterpret_cast<T*>(ptr() + offset(index));
  }

  const T& operator[](uint32_t index) const {
    return *reinterpret_cast<const T*>(ptr() + offset(index));
  }

private:
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_UNIFORM_BUFFER_H
