#ifndef RENDER2D_H
#define RENDER2D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "typedefs.h"
#include "fsmath.h"

struct Vertex {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    float coord[3];
  };
  float texcoord[2];

  Vertex() = default;
  Vertex(float p_x, float p_y, float p_z, float p_tex_u, float p_tex_v);
};

class Texture;

class Render2D {
    uint_t vao;
    uint_t vbo;
    uint_t shader;
    uint_t texture;

    glm::mat4 mvp;

    // Vertex buffer
    Vertex *vertices;

    int vertex_count;
    int vertex_limit;
  
public:
    Render2D(int p_vertex_limit);

    void set_projection_matrix(float p_left, float p_right, float p_bottom, float p_top,
                               float p_znear, float p_zfar);

    void set_mvp(const glm::mat4 &p_mvp);

    void push_vertex(float p_x, float p_y, float p_tex_x, float p_tex_y);
    void draw_buffer();

public:
    void draw_quad(float p_x, float p_y, float p_base, float p_height, Texture *p_texture = nullptr);

public:
    ~Render2D();
};

#endif // RENDER2D_H
