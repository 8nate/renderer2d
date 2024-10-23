#include <stdio.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "render2d.h"
#include "texture.h"

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

  Render2D renderer(6000);

  int width;
  int height;

  Matrix model;
  Matrix view = Matrix::translate(0.0f, 0.0f);
  Matrix projection = Matrix::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
  Matrix mvp = projection * view * model;

  Texture tex = Texture::load("m16.png");

  while (!glfwWindowShouldClose(window)) {
    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    projection = Matrix::ortho(0, (float)width, (float)height, 1.0f);
    mvp = projection * view * model;

    renderer.set_mvp(mvp);

    renderer.draw_quad(320, 240, 300, 300, &tex);

    renderer.draw_buffer();

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
