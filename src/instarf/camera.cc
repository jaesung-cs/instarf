#include <instarf/camera.h>

#include <algorithm>

#include <glm/gtx/transform.hpp>

namespace instarf {

Camera::Camera() = default;

Camera::~Camera() = default;

void Camera::rotate(float dtheta, float dphi) {
  theta_ += dtheta;

  constexpr float eps = 1e-3f;
  phi_ = std::clamp(phi_ + dphi, eps, pi_ - eps);
}

void Camera::rotateByPixels(int dx, int dy) {
  rotate(-sensitivityRotation_ * dx, -sensitivityRotation_ * dy);
}

void Camera::translateByPixels(int dx, int dy) {
  auto sinTheta = std::sin(theta_);
  auto cosTheta = std::cos(theta_);
  auto sinPhi = std::sin(phi_);
  auto cosPhi = std::cos(phi_);

  center_ += radius_ * sensitivityTranslation_ *
             (-static_cast<float>(dx) * glm::vec3(-sinTheta, cosTheta, 0.f) +
              -static_cast<float>(dy) *
                  glm::vec3(cosTheta * cosPhi, sinTheta * cosPhi, -sinPhi));
}

void Camera::zoomByScroll(int scroll) {
  radius_ *= std::exp(-sensitivityZoom_ * scroll);
}

void Camera::moveForward(float x) {
  auto sinTheta = std::sin(theta_);
  auto cosTheta = std::cos(theta_);
  auto sinPhi = std::sin(phi_);
  auto cosPhi = std::cos(phi_);

  center_ -= x * glm::vec3(cosTheta * sinPhi, sinTheta * sinPhi, cosPhi);
}

void Camera::moveRight(float x) {
  auto sinTheta = std::sin(theta_);
  auto cosTheta = std::cos(theta_);
  auto sinPhi = std::sin(phi_);
  auto cosPhi = std::cos(phi_);

  center_ += x * glm::vec3(-sinTheta, cosTheta, 0.f);
}

void Camera::moveUp(float x) { center_ += x * up_; }

glm::vec3 Camera::eye() const {
  auto sinTheta = std::sin(theta_);
  auto cosTheta = std::cos(theta_);
  auto sinPhi = std::sin(phi_);
  auto cosPhi = std::cos(phi_);
  return center_ +
         radius_ * glm::vec3(cosTheta * sinPhi, sinTheta * sinPhi, cosPhi);
}

glm::mat4 Camera::projection() const {
  return glm::perspective(fov_, aspect_, near_, far_);
}

glm::mat4 Camera::view() const { return glm::lookAt(eye(), center_, up_); }

void Camera::setScreenSize(uint32_t width, uint32_t height) {
  aspect_ = static_cast<float>(width) / height;
}

}  // namespace instarf
