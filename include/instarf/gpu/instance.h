#ifndef INSTARF_GPU_INSTANCE_H
#define INSTARF_GPU_INSTANCE_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

struct InstanceInfo {
  std::vector<std::string> extensions;
};

class Instance {
public:
  Instance() = delete;
  explicit Instance(const InstanceInfo& createInfo);
  ~Instance();

  operator VkInstance() const noexcept { return instance_; }

private:
  VkInstance instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT messenger_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_INSTANCE_H
