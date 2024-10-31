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

struct QuadVertex {
  glm::vec3 position;
  glm::vec2 texcoord;
};

struct Transform {
  glm::vec3 position;
  glm::vec2 scale;
  float rotation;
};

class Texture;

class Render2D {
    uint_t vao;
    uint_t vbo;
    uint_t ebo;

    uint_t shader;
    uint_t texture;

    // View projection matrix
    glm::mat4 vp;

    glm::vec4 quad_vertex[6];
    glm::vec2 texture_coord[6];

    // Vertex buffer
    QuadVertex *vertices;
    QuadVertex *next_vertex;

    int vertex_count;
    int index_count;

    int quad_max;
    int vertex_max;
    int index_max;

public:
    Render2D(int p_quad_max);

    void set_view_projection(const glm::mat4 &p_mat);

    void flush();
    void next_batch();

    void draw_buffer();

public:
    void draw_quad(const glm::vec3 &p_position, Texture *p_texture = nullptr,
                   const glm::vec2 &p_scale = glm::vec2(1.0f),
                   float p_rotation = 0.0f);

    void draw_quad(const Transform &p_transform, Texture *p_texture = nullptr);
    
    // deprecated
    void draw_quad(float p_x, float p_y, float p_base, float p_height, Texture *p_texture = nullptr);

    void push_vertex(float p_x, float p_y, float p_tex_u, float p_tex_v);
    void push_vertex(float p_x, float p_y, float p_tex_u, float p_tex_v, const glm::mat4 &p_model);

public:
    ~Render2D();
};

#endif // RENDER2D_H
