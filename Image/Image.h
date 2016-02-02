#pragma once

#ifdef APPLE_COMPILE
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_NONE
#endif

#ifdef WIN_COMPILE
#include "glew.h"
#endif

#include "glfw3.h"

class ImageAccessor;

struct Pixel
{
public:
    unsigned char R,G,B,A;
    Pixel();
    Pixel(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A = 255);
    Pixel(const ImageAccessor accessor);
    void operator=(const ImageAccessor accessor);
};

class Image{
public:
    Image(unsigned int width, unsigned int height);
    ~Image();

    void show(const char * title = 0);
    void save(const char * path);
    ImageAccessor operator()(unsigned int x, unsigned int y);
    GLubyte * Access(unsigned int address);
    unsigned int width() const;
    unsigned int height() const;

private:    
    GLubyte * _buf;
    unsigned int _width, _height, pixelCount;

private:
    //Global GL stuff
    struct GLSupport
    {
        GLuint _vao; ///< vertex array object
        GLuint _pid; ///< GLSL shader program ID
        GLuint _vbo_vpoint; ///< memory buffer
        GLuint _vbo_vtexcoord; ///< memory buffer
        GLuint _tex; ///< Texture ID
        GLSupport()
        {
            _vao = _pid = _vbo_vpoint = _vbo_vtexcoord = _tex = 0;
        }
    };

    GLSupport GLInfo;
    void initializeDraw();
    void cleanupDraw();
};

class ImageAccessor{
public:
    ImageAccessor(Image * that, unsigned int x, unsigned int y);
    void operator=(const Pixel px);
    Pixel read() const;

private:
    Image * _that;
    unsigned int _x, _y, _address;
};

