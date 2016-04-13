/*------------------------------
* Raytrace.h
* Description: A program that generates a basic scene using raycasting.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "Image.h"
#include <vector>
#include "sphere.h"
#include "triangle.h"
#include "quad.h"
#include "light.h"
#include "arealight.h"

#include <iostream>

void SetColor(Pixel & px, Vector3 CalculatedColor)
{
    if (CalculatedColor.x < 0) px.R = 0;
    else if (CalculatedColor.x > 255) px.R = 255;
    else px.R = (unsigned char)CalculatedColor.x;

    if (CalculatedColor.y < 0) px.G = 0;
    else if (CalculatedColor.y > 255) px.G = 255;
    else px.G = (unsigned char)CalculatedColor.y;

    if (CalculatedColor.z < 0) px.B = 0;
    else if (CalculatedColor.z > 255) px.B = 255;
    else px.B = (unsigned char)CalculatedColor.z;

    px.A = 255;
}

std::vector<Object *> pObjectList;
std::vector<Light *> pLightList;
Vector3 AmbientColour(26, 26, 26);
Vector3 BackgroundColor(255, 0, 0);
Vector3 Camera;
int area_light_width = 70;
float area_light_intensity = 0.7f;
int alias = 5;

/*---------------------------------------------------------------------------*
 *
 * Shades the given pixel, according to the diffuse computation and as well as whether or not it can see a light source unobstructed.
 *
 *---------------------------------------------------------------------------*/

Vector3 DiffuseShade(Vector3 Surface, Vector3 Normal, Vector3 objectColour)
{
    Pixel shade;
    Vector3 PixelColour(0,0,0);

    // The area light
    float rand_x = ((float)rand()/(float)(RAND_MAX)) * area_light_width * 2;
    float rand_y = ((float)rand()/(float)(RAND_MAX)) * area_light_width * 2;
    Vector3 LightVector = Minus(Vector3(250-area_light_width + rand_x, 390, 50 + area_light_width + rand_y), Surface);
    float length = sqrt( (LightVector.x*LightVector.x) + (LightVector.y*LightVector.y) + (LightVector.z*LightVector.z));
    LightVector = Normalize(LightVector);
    float DiffuseTerm = DotProduct(LightVector, Normal) * area_light_intensity;
    bool inShadow = false;

    for (int k = 0; k < pObjectList.size(); ++ k)
    {
        float t;
        Vector3 normal;
        bool DoesIntersect = pObjectList[k]->Intersect(Surface, LightVector,
                                                     &t, &normal);
        if (DoesIntersect && t > 0.2f && t < length && !pObjectList[k]->isLight)
        {
            inShadow = true;
            break;
        }
    }

    if (!inShadow) {

        PixelColour = Add(PixelColour, MultiplyScalar(Vector3(40, 40, 40), area_light_intensity));

        if (DiffuseTerm > 0)
        {
            Vector3 PixelDiffuseColour = MultiplyScalar(objectColour, DiffuseTerm);
            PixelColour = Add(PixelColour, PixelDiffuseColour);
        }
    }
    else {
        PixelColour = AmbientColour;
    }



    // Point lighting
    /*for (int n = 0; n < pLightList.size(); ++ n)
    {
            Vector3 LightVector = Minus(pLightList[n]->bulb.Center, Surface);
            float length = sqrt( (LightVector.x*LightVector.x) + (LightVector.y*LightVector.y) + (LightVector.z*LightVector.z));
            LightVector = Normalize(LightVector);
            float DiffuseTerm = DotProduct(LightVector, Normal) * pLightList[n] -> intensity;
            bool inShadow = false;

            for (int k = 0; k < pObjectList.size(); ++ k)
            {
                float t;
                Vector3 normal;
                bool DoesIntersect = pObjectList[k]->Intersect(Surface, LightVector,
                                                             &t, &normal);
                if (DoesIntersect && t > 0.2f && t < length && !pObjectList[k]->isLight)
                {
                    inShadow = true;
                    break;
                }
            }

            if (!inShadow)
                PixelColour = Add(PixelColour, MultiplyScalar(Vector3(40, 40, 40), pLightList[n]->intensity));

            if (DiffuseTerm > 0)
            {
                Vector3 PixelDiffuseColour = MultiplyScalar(objectColour, DiffuseTerm); //used to be diffusecolo
                PixelColour = Add(PixelColour, PixelDiffuseColour);
            }
    }*/

    SetColor(shade, PixelColour);
    return Vector3(shade.R,shade.G,shade.B);
}

Vector3 ShootRay(Vector3 Start, Vector3 Direction) {

    float t_min = 999999;
    Vector3 Normal_min;
    bool HasIntersection = false;
    Vector3 objectColour(0, 0, 0);
    bool isLight = false;
    bool isReflective = false;
    bool isRefractive = false;

    // Intersect the ray with all objects in the scene
    for (int k = 0; k < pObjectList.size(); ++ k)
    {
        float t;
        Vector3 normal;
        bool DoesIntersect = pObjectList[k]->Intersect(Start, Direction,
                                                     &t, &normal);
        if (DoesIntersect)
        {
            HasIntersection = true;
            if (t_min > t)
            {
                t_min = t;
                Normal_min = normal;
                objectColour = pObjectList[k]->colour;
                isLight = pObjectList[k]->isLight;
                isReflective = pObjectList[k]->reflective;
                isRefractive = pObjectList[k]->transclucent;
            }
        }
    }

    // If an intersection is found, determine whether to shade the pixel, reflect the ray, or ignore shading (light orbs)
    if (HasIntersection)
    {
        Vector3 Intersection = MultiplyScalar(Direction, t_min);
        Intersection = Add(Intersection, Start);

        if (isLight)
            return objectColour;
        else if (isReflective)
            return ShootRay(Intersection, Normal_min);
        else if (isRefractive)
        {
            // Calculate Refraction
            float result = 5.0f;
            float n = 1.04f;
            Vector3 N = MultiplyScalar(Normal_min,result);
            float cosI = -DotProduct( N, Direction );
            float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
            if (cosT2 > 0.0f)
            {
                Vector3 T = Add(  MultiplyScalar(Direction,n)  ,  MultiplyScalar(N,   (n * cosI - sqrtf( cosT2 ))    )  );
                return ShootRay(Add(Intersection,MultiplyScalar(T,0.03f)), T);
            }
        }
        else
            return DiffuseShade(Intersection, Normal_min, objectColour);

    }
    else
    {
        // Otherwise, set the pixel to the background color
        return BackgroundColor;
    }
}

/*---------------------------------------------------------------------------*
 *
 * Set up the elements in the scene and begin raycasting procedure.
 *
 *---------------------------------------------------------------------------*/

void RayTraceScene(Image * pImage, int _width, int _height)
{

    // Define the camera position, and place a handful of objects into the scene
    Camera = Vector3(_width/2, _height/2, -550);
    Sphere sphere(Vector3(420, 130, 60), 100, Vector3(0,80,120), true, false);
    Sphere sphereB(Vector3(250, 130, 350), 100, Vector3(0,80,120), false, false);
    Sphere sphereC(Vector3(140, 130, 60), 100, Vector3(180,180,180), false, true);
    Quad scene_floor(Vector3(610, 20, -600), Vector3(610, 20, 700), Vector3(-190, 20, 700), Vector3(-190, 20, -600), Vector3(0,80,120));
    Quad scene_ceiling(Vector3(-190, 420, 700), Vector3(610, 420, 700), Vector3(610, 420, -600), Vector3(-190, 420, -600), Vector3(0,80,120));
    Quad scene_backWall(Vector3(-190, 20, 700), Vector3(610, 20, 700), Vector3(610, 420, 700), Vector3(-190, 420, 700), Vector3(190,190,190));
    Quad scene_rightWall(Vector3(610, 20, -600), Vector3(610, 420, -600), Vector3(610, 420, 700), Vector3(610, 20, 700), Vector3(190,190,190));
    Quad scene_leftWall(Vector3(-190, 20, -600), Vector3(-190, 20, 700), Vector3(-190, 420, 700), Vector3(-190, 420, -600), Vector3(190,190,190));
    Quad scene_behindWall(Vector3(-190, 420, -600), Vector3(610, 420, -600), Vector3(610, 20, -600), Vector3(-190, 20, -600), Vector3(20,20,20));
    AreaLight area_lt(Vector3(250-area_light_width, 390, 50+area_light_width), Vector3(250+area_light_width, 390, 50+area_light_width), Vector3(250+area_light_width, 390, 50-area_light_width), Vector3(250-area_light_width, 390, 50-area_light_width), Vector3(255,255,255));
    //Light lightA(Vector3(150, 300, 200), 10, Vector3(255, 255, 255), 0.4f);
    //Light lightB(Vector3(350, 300, 200), 10, Vector3(255, 255, 255), 0.4f);

    // Push all objects to the objectlist / lightlist
    pObjectList.push_back(&sphere);
    pObjectList.push_back(&sphereB);
    pObjectList.push_back(&sphereC);
    pObjectList.push_back(&scene_ceiling);
    pObjectList.push_back(&area_lt);
    pObjectList.push_back(&scene_floor);
    pObjectList.push_back(&scene_backWall);
    pObjectList.push_back(&scene_rightWall);
    pObjectList.push_back(&scene_leftWall);
    pObjectList.push_back(&scene_behindWall);
    //pObjectList.push_back(&lightA);
    //pObjectList.push_back(&lightB);
    //pLightList.push_back(&lightA);
    //pLightList.push_back(&lightB);

    // Iterate through each pixel on the screen
    for (int i = 0; i < _width; ++ i)
        for (int j = 0; j < _height; ++j)
		{
            Pixel** px_arr = new Pixel*[alias];
            for(int ii=0; ii<alias; ii++)
                px_arr[ii] = new Pixel[alias];

            // Cast a ray from each pixel 4 times (anti-aliasing)
            for(int a = 0; a < alias; a++)
            {
                for(int b = 0; b < alias; b++)
                {
                    // Apply jittering to subdivided rays
                    float max_jitter = 0.2f; //0.25f
                    float jitter_x = ((float)rand()/(float)(RAND_MAX)) * max_jitter;
                    float jitter_y = ((float)rand()/(float)(RAND_MAX)) * max_jitter;

                    // Subdivide each pixel 4 times.
                    Vector3 PixelPosition;
                    PixelPosition = Vector3((float)j + ((float)a / alias), (float)i + ((float)b / alias), 0);
                    PixelPosition = Vector3(PixelPosition.x + jitter_x, PixelPosition.y + jitter_y, 0);
                    Vector3 Direction = Minus(PixelPosition, Camera);
                    Direction = Normalize(Direction);

                    SetColor(px_arr[a][b], ShootRay(Camera, Direction));
                }
            }

            // Average the colors of the 4 rays you casted for this pixel.
            int avgR = 0;
            int avgG = 0;
            int avgB = 0;
            for(int q=0; q<alias; q++)
            {
                for(int w=0; w<alias; w++)
                {
                    avgR += px_arr[q][w].R;
                    avgG += px_arr[q][w].G;
                    avgB += px_arr[q][w].B;
                }
            }
            float _alias = alias * alias;
            Pixel px_avg;
            px_avg.R = avgR / _alias;
            px_avg.G = avgG / _alias;
            px_avg.B = avgB / _alias;
            px_avg.A = 255;
            (*pImage)(_width-i, j) = px_avg;
        }
}
