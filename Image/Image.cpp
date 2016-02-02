#include "Image.h"
#include <vector>
#include <string>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include "png.h"

static const char * vshader_srctxt = " \
        #version 330 core \n\
        in vec3 vpoint; \
        in vec2 vtexcoord; \
        out vec2 uv; \
        void main() { \
            uv = vtexcoord; \
            gl_Position = vec4(vpoint, 1.0); \
        } \
    ";

static const char * fshader_srctxt = " \
        #version 330 core \n\
        out vec3 color; \
        in vec2 uv; \
        uniform sampler2D tex; \
        void main() { color = texture(tex,uv).rgb; } \
        ";

static GLuint compile_shaders(const char * vshader,
                              const char * fshader)
{
    //compile the vertex shader
    GLuint VShaderID = glCreateShader(GL_VERTEX_SHADER);
    const char * vshader_str = vshader;
    glShaderSource(VShaderID, 1, &vshader_str, NULL);
    glCompileShader(VShaderID);

    //check the vertex shader
    GLint success = GL_FALSE;
    int InfoLogLength;
    std::vector<char> errMsg;
    glGetShaderiv(VShaderID, GL_COMPILE_STATUS, &success);
    glGetShaderiv(VShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
     if(!success) {
         errMsg.resize(InfoLogLength);
         glGetShaderInfoLog(VShaderID,
                            InfoLogLength, NULL, &errMsg[0]);
         fprintf(stderr,
                 "Vertex Shader Compilation Error:\n%s\n",
                 &errMsg[0]);
         return 0;
     }

     //compile the fragment shader
     GLuint FShaderID = glCreateShader(GL_FRAGMENT_SHADER);
     char const * fshader_str = fshader;
     glShaderSource(FShaderID, 1, &fshader_str , NULL);
     glCompileShader(FShaderID);

     /// Check Fragment Shader
     glGetShaderiv(FShaderID, GL_COMPILE_STATUS, &success);
     glGetShaderiv(FShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
     if(!success) {
         errMsg.resize(InfoLogLength);
         glGetShaderInfoLog(FShaderID,
                            InfoLogLength, NULL, &errMsg[0]);
         fprintf(stderr, "Fragment Shader Compilation Error:\n%s\n", &errMsg[0]);
         return 0;
     }

     // Link the program
     GLuint ProgramID = glCreateProgram();
     glAttachShader(ProgramID, VShaderID);
     glAttachShader(ProgramID, FShaderID);
     glLinkProgram(ProgramID);

     /// Check the program
     glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
     glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

     if(!success) {
         errMsg.resize(InfoLogLength);
         glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &errMsg[0]);
         fprintf(stderr, "Program Link Failed:\n%s\n", &errMsg[0]);
         return 0;
     }

     glDeleteShader(VShaderID);
     glDeleteShader(FShaderID);

     return ProgramID;
}

static void error_char(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

///////////////////////////////////////////////////////////////////////////////

Pixel::Pixel()
{
    R = G = B = 0;
    A = 1;
}

Pixel::Pixel(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A)
{
    R = _R;
    G = _G;
    B = _B;
    A = _A;
}

Pixel::Pixel(const ImageAccessor accessor)
{
     (*this) = accessor.read();
}

void Pixel::operator=(const ImageAccessor accessor)
{
    (*this) = accessor.read();
}

Image::Image(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;
    pixelCount = _width * _height;
    _buf = new GLubyte[pixelCount * 4];   
}

Image::~Image()
{
    delete [] _buf;
}

void Image::show(const char * title)
{
	fprintf(stderr, "Showing image %s.\n", title);
	glfwSetErrorCallback(error_char);
    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * pWindow = NULL;
    if (title == 0)
        pWindow = glfwCreateWindow(_width, _height, "Image", NULL, NULL);
    else
        pWindow = glfwCreateWindow(_width, _height, title, NULL, NULL);    

    if (!pWindow)
    {
        fprintf(stderr, "\nGLFW Window creation failed.\n");
        glfwTerminate();
        return;
    } else
    {
        
        int major, minor, rev;
        glfwMakeContextCurrent(pWindow);

#ifdef WIN_COMPILE
        glewExperimental=GL_TRUE; 
        if (glewInit() != GLEW_OK)
        {
            fprintf(stderr, "GLEW initialization failed.\n");
            return;
        }
#endif
        glfwGetVersion(&major, &minor, &rev);
        fprintf(stderr, "OpenGL version recieved: %d.%d.%d.\n", major, minor, rev);
    }
    
    glfwSwapInterval(1);
    glfwSetKeyCallback(pWindow, key_callback);

    //After the openGL context is set up, initialize openGL
    initializeDraw();

    //Then load data into a texture
    {
        glGenTextures(1, &GLInfo._tex);
        glBindTexture(GL_TEXTURE_2D, GLInfo._tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width,
                               _height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                               _buf);
        GLuint tex_id = glGetUniformLocation(GLInfo._pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
    }

    glUseProgram(GLInfo._pid);
    glBindVertexArray(GLInfo._vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GLInfo._tex);

    //ok we're ready. Let glfw do the msg loop and draw the quad
    while (!glfwWindowShouldClose(pWindow))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }

     /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(pWindow);
    cleanupDraw();
    glfwTerminate();
}

void Image::save(const char * path)
{
    png_image pngimage_control;
    memset(&pngimage_control, 0, sizeof(png_image));

    pngimage_control.version = PNG_IMAGE_VERSION;
    pngimage_control.width = _width;
    pngimage_control.height = _height;
    pngimage_control.format = PNG_FORMAT_RGBA;
    //pngimage_control.flags = PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB;

    int success = png_image_write_to_file(&pngimage_control, //control str
                                                    path, //file path
                                                    0, //convert_to_8bit
                                                    _buf, //buffer
                                                    0, //row_stride
                                                    NULL);//colormap
    if (!success)
    {
        fprintf(stderr, "Writing png file failed to path: %s.\n", path);
    } else
    {
        fprintf(stderr, "PNG file saved: %s.\n", path);
    }
}

ImageAccessor Image::operator()(unsigned int x, unsigned int y)
{
    return ImageAccessor(this, x, y);
}

GLubyte * Image::Access(unsigned int address)
{
    return _buf + address;
}

unsigned int Image::width() const
{
    return _width;
}

unsigned int Image::height() const
{
    return _height;
}

ImageAccessor::ImageAccessor(Image * that, unsigned int x, unsigned int y)
    :_that(that)
{
    _x = x;
    if (_x >= _that->width()) _x = _that->width() - 1;

    _y = y;
    if (_y >= _that->height()) _y = _that->height() - 1;

    _address = (_x * _that->width() + _y) * 4;
}

void ImageAccessor::operator=(const Pixel px)
{
    GLubyte * ptr = _that->Access(_address);
    *ptr = px.R;
    *(ptr + 1) = px.G;
    *(ptr + 2) = px.B;
    *(ptr + 3) = px.A;
}

Pixel ImageAccessor::read() const
{
    GLubyte * ptr = _that->Access(_address);
    Pixel px;
    px.R = *ptr;
    px.G = *(ptr + 1);
    px.B = *(ptr + 2);
    px.A = *(ptr + 3);
    return px;
}

void Image::initializeDraw()
{
    //Shader Program
    GLInfo._pid = compile_shaders(vshader_srctxt, fshader_srctxt);
    glUseProgram(GLInfo._pid);

    //VAO
    glGenVertexArrays(1, &GLInfo._vao);
    glBindVertexArray(GLInfo._vao);

    //VBO_VPOINT
    ///--- Vertex coordinates
    {
        const GLfloat vpoint[] = { /*V0*/ -1.0f, -1.0f, 0.0f,
                                   /*V1*/ +1.0f, -1.0f, 0.0f,
                                   /*V2*/ -1.0f, +1.0f, 0.0f,
                                   /*V3*/ +1.0f, +1.0f, 0.0f };
        ///--- Buffer
        glGenBuffers(1, &GLInfo._vbo_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, GLInfo._vbo_vpoint);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

        ///--- Attribute
        GLuint vpoint_id = glGetAttribLocation(GLInfo._pid, "vpoint");
        glEnableVertexAttribArray(vpoint_id);
        glVertexAttribPointer(vpoint_id, 3, GL_FLOAT,
                              GL_FALSE, 0, (void *)0);
    }

    //VBO_VTEXCOORD
    /// NOTE: swaps Y upside down to match top-left image coordinates system
    ///       this way Image(0,0) is the top-left pixel in screen
    {
        const GLfloat vtexcoord[] = { /*v0*/ 0.0f, 1.0f,
                                      /*v1*/ 1.0f, 1.0f,
                                      /*v2*/ 0.0f, 0.0f,
                                      /*v3*/ 1.0f, 0.0f };

        ///--- Buffer
        glGenBuffers(1, &GLInfo._vbo_vtexcoord);
        glBindBuffer(GL_ARRAY_BUFFER, GLInfo._vbo_vtexcoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

        ///--- Attribute
        GLuint vtexcoord_id = glGetAttribLocation(GLInfo._pid, "vtexcoord");
        glEnableVertexAttribArray(vtexcoord_id);
        glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT,
                              GL_FALSE, 0, (void *)0);
    }
}

void Image::cleanupDraw()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &GLInfo._tex);

    glBindVertexArray(0);
    glDeleteBuffers(1, &GLInfo._vbo_vpoint);
    glDeleteBuffers(1, &GLInfo._vbo_vtexcoord);

    glUseProgram(0);
    glDeleteProgram(GLInfo._pid);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &GLInfo._vao);
}
