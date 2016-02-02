#include "Image.h"

#include "raytrace.h"

/*
 * Raycasts a scene of given width and height.
 */

int main(int, char**){

    int _width = 512;
    int _height = 512;

    Image raytraceImage(_width, _height);
    RayTraceScene(&raytraceImage, _width, _height);
    raytraceImage.show("CSC 305 RayCaster - Harrison Walters");
    
}
