#ifndef INSTARF_ENGINE_H
#define INSTARF_ENGINE_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace instarf {

struct EngineCreateInfo {
  std::vector<std::string> instanceExtensions;
};

class Engine {
public:
  Engine() = delete;
  explicit Engine(const EngineCreateInfo& engineInfo);
  ~Engine();

  auto instance() const noexcept { return instance_; }
  auto device() const noexcept { return device_; }
  auto queue() const noexcept { return queue_; }

private:
  VkInstance instance_;
  VkDebugUtilsMessengerEXT messenger_;

  VkPhysicalDevice physicalDevice_;
  VkDevice device_;
  int queueIndex_ = -1;
  VkQueue queue_;
};

}  // namespace instarf

#endif  // INSTARF_ENGINE_H
