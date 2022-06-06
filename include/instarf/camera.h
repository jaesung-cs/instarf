#ifndef INSTARF_CAMERA_H
#define INSTARF_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace instarf {

class Camera {
public:
  Camera();
  ~Camera();

  void rotate(float dtheta, float dphi);
  void rotateByPixels(int dx, int dy);
  void translateByPixels(int dx, int dy);
  void zoomByScroll(int scroll);

  void moveForward(float x);
  void moveRight(float x);
  void moveUp(float x);

  glm::vec3 eye() const;
  glm::mat4 projection() const;
  glm::mat4 view() const;

  auto radius() const noexcept { return radius_; }
  auto aspect() const noexcept { return aspect_; }
  auto fov() const noexcept { return fov_; }
  auto center() const noexcept { return center_; }

  void setScreenSize(uint32_t width, uint32_t height);

private:
  static constexpr float pi_ = glm::pi<float>();

  static constexpr glm::vec3 up_{0.f, 0.f, 1.f};

  float theta_ = pi_ / 4.f;
  float phi_ = pi_ / 4.f;  // [0, pi]

  glm::vec3 center_{0.f, 0.f, 0.f};
  float radius_ = 3.f;

  float fov_ = pi_ / 3.f;
  float aspect_ = 1.f;
  float near_ = 0.1f;
  float far_ = 100.f;

  float sensitivityZoom_ = 0.1f;
  float sensitivityTranslation_ = 0.003f;
  float sensitivityRotation_ = 0.003f;
};

}  // namespace instarf

#endif  // INSTARF_CAMERA_H
