#include <stdio.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "render2d.h"
#include "texture.h"

struct Vector2 {
  union {
    struct {
      float x;
      float y;
    };
    float coords[2];
  };

  Vector2() {
    x = 0;
    y = 0;
  }
};

Vector2 camera_offset;

static bool move_up;
static bool move_down;
static bool move_left;
static bool move_right;

static const float camera_speed = 3.0f;

void _win_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_W) {
    move_up = action;
  }

  if (key == GLFW_KEY_S) {
    move_down = action;
  }

  if (key == GLFW_KEY_A) {
    move_left = action;
  }

  if (key == GLFW_KEY_D) {
    move_right = action;
  }
}

int main(void) {
  if (!glfwInit()) {
    return -1;
  }

  GLFWwindow *window;
  window = glfwCreateWindow(640, 480, "BatchRenderer::World2D", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (gl3wInit() || !gl3wIsSupported(3, 3)) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, _win_key_callback);
  glfwSwapInterval(1);

  Render2D renderer(6000);

  int width;
  int height;

  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
  glm::mat4 projection = glm::ortho(0.0f, 640.0f, 480.0f, 1.0f);

  Texture ak5 = Texture::load("assets/ak5.png");
  Texture ak47 = Texture::load("assets/ak47.png");
  Texture awm = Texture::load("assets/awm.png");
  Texture m16 = Texture::load("assets/m16.png");

  Transform t;
  t.position = glm::vec3(320.0f, 300.0f, 0.0f);
  t.scale = glm::vec2(1.0f);

  float r = 0.0f;

  int fc;
  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    fc++;

    double time = glfwGetTime();
    if (time - last_time >= 1.0) {
      printf("\rFPS: %d", fc);
      fc = 0;
      last_time = time;
    }

    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    view = glm::translate(glm::mat4(1.0f), glm::vec3(camera_offset.x, camera_offset.y, 0.0f));
    projection = glm::ortho(0.0f, (float)width, (float)height, 1.0f);

    renderer.set_view_projection(projection * view);

    renderer.draw_quad(glm::vec3(200, 50, 0.0f), &ak5, glm::vec2(0.3125, 0.3125));
    renderer.draw_quad(glm::vec3(250, 50, 0.0f), &ak47, glm::vec2(0.3125, 0.3125));
    renderer.draw_quad(glm::vec3(300, 50, 0.0f), &awm, glm::vec2(0.3125, 0.3125));
    renderer.draw_quad(glm::vec3(350, 50, 0.0f), &m16, glm::vec2(0.3125, 0.3125));

    camera_offset.x += (move_left - move_right) * camera_speed;
    camera_offset.y += (move_up - move_down) * camera_speed;

    r += 0.005;

    // renderer.draw_quad(200, 50, 200, 200, &ak5);
    // renderer.draw_quad(100, 240, 200, 200, &ak47);
    // renderer.draw_quad(300, 90, 200, 200, &awm);
    // renderer.draw_quad(350, 70, 200, 200, &m16);

    renderer.draw_buffer();

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
