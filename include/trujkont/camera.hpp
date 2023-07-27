#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

// Image Mark
// [:https://learnopengl.com/img/getting-started/camera_axes.png dimensions 500 500 offset 5 2 show_always pad:]

class Camera
{
public:
  Camera(GLFWwindow* const window);

  auto update(long long delta_time, float aspect_ratio) -> std::pair<glm::mat4, glm::mat4>;

  auto process_input(long long delta_time) -> void;

  glm::vec3 position = glm::vec3(0.);

private:
  GLFWwindow* window;

  float speed = 0.5;
  bool orthogonal = false;

  glm::vec3 reverse_direction = glm::vec3(0., 0., -1.);
  glm::vec3 up_vector = glm::vec3(0., 1., 0.);
};
