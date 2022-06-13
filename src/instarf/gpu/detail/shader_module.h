#ifndef INSTARF_GPU_DETAIL_SHADER_MODULE_H
#define INSTARF_GPU_DETAIL_SHADER_MODULE_H

#include <string>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

VkShaderModule createShaderModule(VkDevice device, const std::string& filepath);

}
}

#endif  // INSTARF_GPU_DETAIL_SHADER_MODULE_H
