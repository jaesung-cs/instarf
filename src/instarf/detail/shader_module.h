#ifndef INSTARF_DETAIL_SHADER_MODULE_H
#define INSTARF_DETAIL_SHADER_MODULE_H

#include <string>

#include <vulkan/vulkan.h>

namespace instarf {

VkShaderModule createShaderModule(VkDevice device, const std::string& filepath);

}

#endif  // INSTARF_DETAIL_SHADER_MODULE_H
