#ifndef INSTARF_ENGINE_H
#define INSTARF_ENGINE_H

#include <memory>
#include <vector>
#include <string>

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace instarf {

struct EngineInfo {
  std::vector<std::string> instanceExtensions;
};

class Engine {
public:
  Engine() = default;
  explicit Engine(const EngineInfo& createInfo);
  ~Engine() = default;

  VkInstance instance() const;
  VkDevice device() const;
  int queueIndex() const;
  VkQueue queue() const;
  VmaAllocator allocator() const;

  void waitIdle();

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace instarf

#endif  // INSTARF_ENGINE_H
