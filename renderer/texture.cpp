#include "texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "GL/gl3w.h"

Texture Texture::load(const char *p_filename, bool p_use_mipmaps) {
    int width = 0;
    int height = 0;
    int channels = 0;
    
    uint_t id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    void *data = stbi_load(p_filename, &width, &height, &channels, 0);
    
    if (!data) {
        return Texture();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    
    
    if (p_use_mipmaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(data);

    // Create texture object
    Texture tex;

    tex.id = id;
    tex.width = width;
    tex.height = height;

    return tex;
}