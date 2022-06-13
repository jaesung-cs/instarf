#ifndef INSTARF_GPU_INSTANCE_H
#define INSTARF_GPU_INSTANCE_H

#include <memory>
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
  Instance() = default;
  explicit Instance(const InstanceInfo& createInfo);
  ~Instance() = default;

  operator VkInstance() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_INSTANCE_H
