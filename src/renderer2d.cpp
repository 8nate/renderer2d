#include <iostream>
#include <sstream>
#include <fstream>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"



class Shader {

public:
    GLuint id;

    enum UniformType {
        INT,
        REAL,
    };

public:
    std::string get_shader_source(const char *p_path) {
        std::ifstream file;
        std::string source;

        file.open(p_path, std::ios::binary);

        if (!file.is_open()) {
            return "";
        }

        std::ostringstream stream;
        stream << file.rdbuf();
        file.close();

        return stream.str();
    }

    void open(const char *p_vert_path, const char *p_frag_path) {
        const char *vert_source = get_shader_source(p_vert_path).c_str();
        const char *frag_source = get_shader_source(p_frag_path).c_str();

        int result;
        char log[512];

        GLuint vert;
        GLuint frag;

        vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vert_source, NULL);
        glCompileShader(vert);

        glGetShaderiv(vert, GL_COMPILE_STATUS, &result);

        if (!result) {
            glGetShaderInfoLog(vert, 512, NULL, log);
            std::cout << "Shader compilation failed!\n" << log << std::endl;
        }

        frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &frag_source, NULL);
        glCompileShader(frag);

        glGetShaderiv(frag, GL_COMPILE_STATUS, &result);

        if (!result) {
            glGetShaderInfoLog(frag, 512, NULL, log);
            std::cout << "Shader compilation failed!\n" << log << std::endl;
        }

        id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id, frag);
        glLinkProgram(id);

        glGetProgramiv(id, GL_LINK_STATUS, &result);
        
        if (!result) {
            glGetProgramInfoLog(id, 512, NULL, log);
            std::cout << "Shader linking failed!\n" << log << std::endl;
        }

        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    void enable() {
        glUseProgram(id);
    }

    void uniformi(const std::string &p_name, int p_value) {
        glUniform1i(glGetUniformLocation(id, p_name.c_str()), p_value);
    }

    void uniformf(const std::string &p_name, float p_value) {
        glUniform1f(glGetUniformLocation(id, p_name.c_str()), p_value);
    }

    ~Shader() {
        glDeleteProgram(id);
    }
};


class VertexBuffer {

public:
    GLuint id;

public:
    VertexBuffer(GLuint p_size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, p_size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer(GLuint p_size, float *p_vertices) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, p_size, p_vertices, GL_STATIC_DRAW);
    }

    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void set_data(GLuint p_size, float *p_vertices) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, p_size, p_vertices);
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &id);
    }
};


float vertices[] = {
    // first
    -1.0f, -1.0f, 0.0f, // bl
    -1.0f, 0.0f, 0.0f, // tl,
    0.0f, -1.0f, 0.0f, // br
    0.0f, 0.0f, 0.0f, // tr
};

GLuint indices[] = {
    0, 1, 2,
    1, 3, 2,
};

class Scene {
    Shader shader_red;
    GLuint VAO, VBO, EBO;

    VertexBuffer buf = VertexBuffer(512);

public:
    Scene() {
        shader_red.open("shaders/default.vert", "shaders/red.frag");

        // Generate VAOs and buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        buf.set_data(sizeof(vertices), vertices);

        // create index buffer
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
    }

    void draw() {
        shader_red.enable();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Scene() {
        glDeleteVertexArrays(1, &VAO);
    }
};


void fb_size_callback(GLFWwindow *p_window, int p_width, int p_height) {
    glViewport(0, 0, p_width, p_height);
}


class Window {
    GLFWwindow *_window;
    Scene *_scene;

public:

    int open(int p_width, int p_height) {
        _window = glfwCreateWindow(p_width, p_height, "Renderer2D::Window", NULL, NULL);

        if (!_window) {
            return 1;
        }

        glfwMakeContextCurrent(_window);

        if (gl3wInit() || !gl3wIsSupported(3, 2)) {
            return 1;
        }

        glViewport(0, 0, p_width, p_height);
        glfwSetFramebufferSizeCallback(_window, &fb_size_callback);

        return 0;
    }

    void set_scene(Scene *p_scene) {
        _scene = p_scene;
    }

    void update() {
        glClear(GL_COLOR_BUFFER_BIT);

        _scene->draw();
        
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    int should_close() {
        return glfwWindowShouldClose(_window);
    }
};




int main(void) {
    std::cout << "-- Initializing GLFW" << std::endl;

    if (!glfwInit()) {
        std::cout << "Failed to initialize glfw" << std::endl;
        return 1;
    }

    std::cout << "-- Initialized GLFW" << std::endl;

    Window window;

    if (window.open(640, 480) != 0) {
        std::cout << "Failed to open window" << std::endl;

        glfwTerminate();
        return 1;
    }
    
    Scene scene;
    window.set_scene(&scene);

    while (!window.should_close()) {
        window.update();
    }

    std::cout << "Finished" << std::endl;

    glfwTerminate();
    return 0;
}