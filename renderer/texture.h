#ifndef TEXTURE_H
#define TEXTURE_H

#include "typedefs.h"

class Texture {

public:
    uint_t id;
    int width;
    int height;

public:
    static Texture load(const char *p_filename);
};

#endif // TEXTURE_H