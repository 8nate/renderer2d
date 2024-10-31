#include <stddef.h>
#include <stdio.h>

#include "render2d.h"
#include "texture.h"

#include "GL/gl3w.h"

#include <iostream>


static const char *vert_shader_src = \
"#version 330 core\n"
"layout (location = 0) in vec3 a_position;\n"
"layout (location = 1) in vec2 a_texcoord;\n"
"uniform mat4 u_vp;\n"
"out vec2 f_texcoord;\n"
"void main() {\n"
"   gl_Position = u_vp * vec4(a_position, 1.0);\n"
"   f_texcoord = a_texcoord;\n"
"}\n";

static const char *frag_shader_src = \
"#version 330 core\n"
"out vec4 frag_color;\n"
"in vec2 f_texcoord;\n"
"uniform sampler2D u_texture;\n"
"void main() {\n"
"   frag_color = texture(u_texture, f_texcoord);\n"
"}\n";


Vertex::Vertex(float p_x, float p_y, float p_z, float p_tex_u, float p_tex_v) {
    x = p_x;
    y = p_y;
    z = p_z;
    texcoord[0] = p_tex_u;
    texcoord[1] = p_tex_v;
}

static uint_t compile_shader(const char **p_source, GLenum p_shader_type) {
    int success;
    char error_log[512];
    uint_t shader;

    shader = glCreateShader(p_shader_type);
    glShaderSource(shader, 1, p_source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, error_log);
        printf("Shader Compilation Failed!\n%s\n", error_log);
    }

    return shader;
}

Render2D::Render2D(int p_quad_max) : quad_max(p_quad_max) {
    vertex_max = quad_max * 4;
    index_max = quad_max * 6;

    // Load shader
    uint_t vshader, fshader;

    vshader = compile_shader(&vert_shader_src, GL_VERTEX_SHADER);
    fshader = compile_shader(&frag_shader_src, GL_FRAGMENT_SHADER);
    
    shader = glCreateProgram();
    glAttachShader(shader, vshader);
    glAttachShader(shader, fshader);
    glLinkProgram(shader);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    quad_vertex[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
    quad_vertex[1] = { -1.0f,  1.0f, 0.0f, 1.0f };
    quad_vertex[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
    quad_vertex[3] = {  1.0f,  1.0f, 0.0f, 1.0f };
    quad_vertex[4] = {  1.0f, -1.0f, 0.0f, 1.0f };
    quad_vertex[5] = { -1.0f, -1.0f, 0.0f, 1.0f };

    texture_coord[0] = { 0.0f, 0.0f };
    texture_coord[1] = { 0.0f, 1.0f };
    texture_coord[2] = { 1.0f, 1.0f };
    texture_coord[3] = { 1.0f, 1.0f };
    texture_coord[4] = { 1.0f, 0.0f };
    texture_coord[5] = { 0.0f, 0.0f };

    vertices = new QuadVertex[vertex_max];

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_max * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

    // Create index buffer
    uint32_t *indices = new uint32_t[index_max];
    uint32_t offset = 0;

    for (uint32_t i = 0; i < index_max; i += 6) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);   

    delete[] indices;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)offsetof(QuadVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)offsetof(QuadVertex, texcoord));
}

void Render2D::set_view_projection(const glm::mat4 &p_mat) {
    vp = p_mat;
}

void Render2D::push_vertex(float p_x, float p_y, float p_tex_u, float p_tex_v) {
    if (vertex_count >= vertex_max) {
        draw_buffer();
    }

    next_vertex->position.x = p_x;
    next_vertex->position.y = p_y;
    next_vertex->position.z = 0.0f;

    next_vertex->texcoord.x = p_tex_u;
    next_vertex->texcoord.y = p_tex_v;

    next_vertex++;
    vertex_count++;
}

void Render2D::push_vertex(float p_x, float p_y, float p_tex_u, float p_tex_v, const glm::mat4 &p_model) {
    glm::vec4 pos = glm::vec4(p_x, p_y, 0.0f, 1.0f);

    next_vertex->position = p_model * pos;
    next_vertex->texcoord[0] = p_tex_u;
    next_vertex->texcoord[1] = p_tex_v;

    next_vertex++;
    vertex_count++;
}


void Render2D::flush() {
    glUseProgram(shader);

    if (texture) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "u_texture"), 0);
    }

    glUniformMatrix4fv(glGetUniformLocation(shader, "u_vp"), 1, GL_FALSE, glm::value_ptr(vp));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(QuadVertex) * vertex_count, vertices);

    glBindVertexArray(vao);

    // glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Render2D::next_batch() {
    next_vertex = vertices;
    vertex_count = 0;
    index_count = 0;
}

void Render2D::draw_buffer() {
    flush();
    next_batch();
}

void Render2D::draw_quad(const glm::vec3 &p_position, Texture *p_texture, const glm::vec2 &p_scale, float p_rotation) {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), p_position)
        * glm::rotate(glm::mat4(1.0f), p_rotation, { 0.0f, 0.0f, 1.0f })
        * glm::scale(glm::mat4(1.0f), { p_scale.x, p_scale.y, 1.0f });
    
    if (vertex_count >= vertex_max) {
        draw_buffer();
    }

    float offx, offy;

    if (p_texture == nullptr) {
        if (texture != 0) {
            draw_buffer();
        }
        texture = 0;
        offx = offy = 0;
    }

    else {
        if (p_texture->id != texture) {
            draw_buffer();
        }

        texture = p_texture->id;

        offx = p_texture->width / 2;
        offy = p_texture->height / 2;
    }

    for (int i = 0; i < 6; i++) {
        glm::vec4 vertex = quad_vertex[i] * glm::vec4(offx, offy, 0.0f, 1.0f);
        next_vertex->position = t * vertex;
        next_vertex->texcoord = texture_coord[i];
        next_vertex++;
        vertex_count++;
    }
}

void Render2D::draw_quad(const Transform &p_transform, Texture *p_texture) {
    draw_quad(p_transform.position, p_texture, p_transform.scale, p_transform.rotation);
}

void Render2D::draw_quad(float p_x, float p_y, float p_base, float p_height, Texture *p_texture) {
    float offx = p_base / 2;
    float offy = p_height / 2;

    if (p_texture == nullptr) {
        if (texture != 0)
            draw_buffer();
        texture = 0;
    }

    else if (p_texture->id != texture) {
        draw_buffer();
        texture = p_texture->id;
    }

    // First triangle
    push_vertex(p_x - offx, p_y - offy, 0.0, 0.0); // bottom left
    push_vertex(p_x - offx, p_y + offy, 0.0, 1.0); // top left
    push_vertex(p_x + offx, p_y + offy, 1.0, 1.0); // top right

    // Second triangle
    push_vertex(p_x + offx, p_y + offy, 1.0, 1.0); // top right
    push_vertex(p_x + offx, p_y - offy, 1.0, 0.0); // bottom right
    push_vertex(p_x - offx, p_y - offy, 0.0, 0.0); // bottom left
}


Render2D::~Render2D() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    delete[] vertices;
}