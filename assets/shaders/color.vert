#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (set = 0, binding = 0) uniform CameraUbo {
  mat4 projection;
  mat4 view;
} camera;

layout (push_constant) uniform ModelPush {
  mat4 model;
} model;

layout (location = 0) out vec3 vColor;

void main() {
  gl_Position = camera.projection * camera.view * model.model * vec4(position, 1.f);
  vColor = color;
}
