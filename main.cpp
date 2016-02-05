/*------------------------------
* Main.cpp
* Description: A program that raycasts a scene with a given width and height.
*
* By: Harrison Walters
-------------------------------*/

#include "Image.h"
#include "raytrace.h"

int main(int, char**){

    int _width = 512;
    int _height = 512;

    Image raytraceImage(_width, _height);
    RayTraceScene(&raytraceImage, _width, _height);
    raytraceImage.show("CSC 305 RayCaster - Harrison Walters");
    
}
