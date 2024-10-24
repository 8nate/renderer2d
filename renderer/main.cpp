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

void _win_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (action != GLFW_PRESS) {
    return;
  }
  
  const float CAMERA_SPEED = 20.0f;

  switch (key) {
    case GLFW_KEY_W: {
      camera_offset.y += CAMERA_SPEED;
      break;
    }
    case GLFW_KEY_S: {
      camera_offset.y -= CAMERA_SPEED;
      break;
    }
    case GLFW_KEY_A: {
      camera_offset.x += CAMERA_SPEED;
      break;
    }
    case GLFW_KEY_D: {
      camera_offset.x -= CAMERA_SPEED;
      break;
    }
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

  Render2D renderer(6000);

  int width;
  int height;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
  glm::mat4 projection = glm::ortho(0.0f, 640.0f, 480.0f, 1.0f);
  glm::mat4 mvp = projection * view * model;

  Texture ak5 = Texture::load("assets/ak5.png");
  Texture ak47 = Texture::load("assets/ak47.png");
  Texture awm = Texture::load("assets/awm.png");
  Texture m16 = Texture::load("assets/m16.png");

  while (!glfwWindowShouldClose(window)) {
    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    view = glm::translate(glm::mat4(1.0f), glm::vec3(camera_offset.x, camera_offset.y, 0.0f));
    projection = glm::ortho(0.0f, (float)width, (float)height, 1.0f);
    mvp = projection * view * model;

    renderer.set_mvp(mvp);

    renderer.draw_quad(200, 50, 200, 200, &ak5);
    renderer.draw_quad(100, 240, 200, 200, &ak47);
    renderer.draw_quad(300, 90, 200, 200, &awm);
    renderer.draw_quad(350, 70, 200, 200, &m16);

    renderer.draw_buffer();

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
