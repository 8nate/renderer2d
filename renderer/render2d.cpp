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
"uniform mat4 u_mvp;\n"
"out vec2 f_texcoord;\n"
"void main() {\n"
"   gl_Position = u_mvp * vec4(a_position, 1.0);\n"
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

Render2D::Render2D(int p_vertex_limit) : vertex_limit(p_vertex_limit) {
    
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

    vertices = new Vertex[vertex_limit];

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_limit * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, coord));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoord));
}

void Render2D::set_mvp(const glm::mat4 &p_mvp) {
    mvp = p_mvp;
}

void Render2D::push_vertex(float p_x, float p_y, float p_tex_x, float p_tex_y) {
    if (vertex_count >= vertex_limit) {
        draw_buffer();
    }

    vertices[vertex_count++] = Vertex(p_x, p_y, 1.0f, p_tex_x, p_tex_y);
}

void Render2D::draw_buffer() {
    glUseProgram(shader);

    if (texture) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "u_texture"), 0);
    }

    glUniformMatrix4fv(glGetUniformLocation(shader, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertex_count, vertices);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    glBindTexture(GL_TEXTURE_2D, 0);

    vertex_count = 0;
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