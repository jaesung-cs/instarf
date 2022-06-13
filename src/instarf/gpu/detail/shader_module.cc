#include <instarf/gpu/detail/shader_module.h>

#include <vector>
#include <fstream>

namespace instarf {
namespace gpu {

VkShaderModule createShaderModule(VkDevice device,
                                  const std::string& filepath) {
  VkShaderModule shaderModule;

  std::vector<uint8_t> code;
  {
    std::ifstream in(filepath, std::ios::ate | std::ios::binary);

    uint64_t filesize = in.tellg();
    code.resize(filesize);

    in.seekg(0);
    in.read(reinterpret_cast<char*>(code.data()), filesize);
  }

  VkShaderModuleCreateInfo shaderModuleInfo = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  shaderModuleInfo.codeSize = code.size();
  shaderModuleInfo.pCode = reinterpret_cast<uint32_t*>(code.data());
  vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule);
  return shaderModule;
}

}  // namespace gpu
}  // namespace instarf
