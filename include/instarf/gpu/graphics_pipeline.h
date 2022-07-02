#ifndef INSTARF_GPU_GRAPHICS_PIPELINE_H
#define INSTARF_GPU_GRAPHICS_PIPELINE_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

struct GraphicsPipelineInfo {
  std::string directory;
  std::string name;
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;
  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
  VkPipelineLayout layout;
  VkRenderPass renderPass;
  uint32_t subpass = 0;
};

class GraphicsPipeline {
public:
  GraphicsPipeline() = delete;
  GraphicsPipeline(const Device& device, const GraphicsPipelineInfo& createInfo);
  ~GraphicsPipeline();

  operator VkPipeline() const noexcept { return pipeline_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_GRAPHICS_PIPELINE_H
