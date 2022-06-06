#ifndef INSTARF_UNIFORM_BUFFER_H
#define INSTARF_UNIFORM_BUFFER_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

class UniformBufferBase {
public:
  UniformBufferBase() = default;
  UniformBufferBase(Engine engine, uint32_t elementSize, uint32_t size);
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

  UniformBuffer(Engine engine, uint32_t size)
      : UniformBufferBase(engine, sizeof(T), size) {}

  ~UniformBuffer() override = default;

  T& operator[](uint32_t index) {
    return *reinterpret_cast<T*>(ptr() + offset(index));
  }

  const T& operator[](uint32_t index) const {
    return *reinterpret_cast<const T*>(ptr() + offset(index));
  }

private:
};

}  // namespace instarf

#endif  // INSTARF_UNIFORM_BUFFER_H
