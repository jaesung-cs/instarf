#ifndef INSTARF_GPU_COMPUTE_PIPELINE_H
#define INSTARF_GPU_COMPUTE_PIPELINE_H

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
  ComputePipeline() = delete;
  ComputePipeline(const Device& device, const ComputePipelineInfo& createInfo);
  ~ComputePipeline();

  operator VkPipeline() const noexcept { return pipeline_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_COMPUTE_PIPELINE_H
