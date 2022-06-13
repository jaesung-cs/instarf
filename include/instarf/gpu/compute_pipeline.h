#ifndef INSTARF_GPU_COMPUTE_PIPELINE_H
#define INSTARF_GPU_COMPUTE_PIPELINE_H

#include <memory>
#include <string>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

struct ComputePipelineInfo {
  std::string directory;
  std::string name;
  VkPipelineLayout layout;
};

class ComputePipeline {
public:
  ComputePipeline() = default;
  ComputePipeline(Device device, const ComputePipelineInfo& createInfo);
  ~ComputePipeline() = default;

  operator VkPipeline() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_COMPUTE_PIPELINE_H
