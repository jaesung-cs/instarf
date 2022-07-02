#ifndef INSTARF_SHADER_CAMERA_UBO_H
#define INSTARF_SHADER_CAMERA_UBO_H

#include <glm/glm.hpp>

namespace instarf {

struct CameraUbo {
  alignas(16) glm::mat4 projection;
  alignas(16) glm::mat4 view;
};

}  // namespace instarf

#endif  // INSTARF_SHADER_CAMERA_UBO_H
