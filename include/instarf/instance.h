#ifndef INSTARF_INSTANCE_H
#define INSTARF_INSTANCE_H

#include <memory>
#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace instarf {

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

}  // namespace instarf

#endif  // INSTARF_INSTANCE_H
