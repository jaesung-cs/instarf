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
