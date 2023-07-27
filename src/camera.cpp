#include "trujkont/camera.hpp"

namespace
{

float yaw = -90.f;
float pitch = 0.f;

bool process_user_input = false;

auto last_x = 400.f;
auto last_y = 300.f;

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    process_user_input = true;
  }
}

void mouse_move_callback(GLFWwindow* const window, double const pos_x, double const pos_y)
{
  if(not process_user_input) return;

  auto delta_x = static_cast<float>(pos_x) - last_x;
  auto delta_y = last_y - static_cast<float>(pos_y);
  last_x = pos_x;
  last_y = pos_y;

  float const sensitivity = .05f;
  delta_x *= sensitivity;
  delta_y *= sensitivity;

  yaw += delta_x;
  pitch = glm::clamp(pitch + delta_y, -89.0f, 89.0f);
}

} // namespace

Camera::Camera(GLFWwindow* const window)
  : window(window)
{
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetMouseButtonCallback(window, mouse_click_callback);
}

auto Camera::update(long long delta_time, float aspect_ratio) -> std::pair<glm::mat4, glm::mat4>
{
  process_input(delta_time);

  glm::vec3 direction = glm::vec3(0.);
  direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction.y = std::sin(glm::radians(pitch));
  direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

  reverse_direction = glm::normalize(direction);

  auto const frustrum = orthogonal
                          ? glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f)
                          : glm::perspective(glm::radians(45.f), aspect_ratio, 0.1f, 100.0f);

  return {
    glm::lookAt(position, position + reverse_direction, up_vector),
    frustrum
  };
}

auto Camera::process_input(long long delta_time) -> void
{
  if(not process_user_input) return;

  auto const camera_speed = speed * static_cast<float>(delta_time) / 10'000;

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    position += (camera_speed * reverse_direction);

  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    position -= camera_speed * reverse_direction;

  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    position -= glm::normalize(glm::cross(reverse_direction, up_vector)) * camera_speed;

  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    position += glm::normalize(glm::cross(reverse_direction, up_vector)) * camera_speed;

  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    orthogonal = not orthogonal;

  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    process_user_input = false;
  }
}
