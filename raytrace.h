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
Vector3 AmbientColour(0, 0, 0);
Vector3 BackgroundColor(255, 0, 0);
Vector3 Camera;

/*---------------------------------------------------------------------------*
 *
 * Shades the given pixel, according to the diffuse computation and as well as whether or not it can see a light source unobstructed.
 *
 *---------------------------------------------------------------------------*/

Pixel DiffuseShade(Vector3 Surface, Vector3 Normal, Vector3 objectColour)
{
    Pixel shade;
    Vector3 PixelColour = AmbientColour;

    for (int n = 0; n < pLightList.size(); ++ n)
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
    }

    SetColor(shade, PixelColour);
    return shade;
}

/*---------------------------------------------------------------------------*
 *
 * Bounces a ray off of a reflective surface, and collects shading information from the resulting intersection
 *
 *---------------------------------------------------------------------------*/

Pixel calcReflection(Vector3 Surface, Vector3 Normal) {

    for (int k = 0; k < pObjectList.size(); ++ k)
    {
        float t;
        Vector3 normal;
        bool DoesIntersect = pObjectList[k]->Intersect(Surface, Normal,
                                                             &t, &normal);
        if (DoesIntersect && t > 0.2f && !pObjectList[k]->isLight)
        {
            Vector3 Intersection = MultiplyScalar(Normal, t);
            Intersection = Add(Surface, Intersection);
            return DiffuseShade(Intersection,normal,pObjectList[k]->colour);
        }
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

    Sphere sphere(Vector3(420, 130, 180), 100, Vector3(0,80,120), false);
    Sphere sphereB(Vector3(250, 130, 350), 100, Vector3(0,80,120), true);
    Sphere sphereC(Vector3(40, 100, 200), 70, Vector3(180,180,180), false);
    Quad scene_floor(Vector3(610, 20, -600), Vector3(610, 20, 700), Vector3(-190, 20, 700), Vector3(-190, 20, -600), Vector3(0,80,120));
    Quad scene_ceiling(Vector3(-190, 420, 700), Vector3(610, 420, 700), Vector3(610, 420, -600), Vector3(-190, 420, -600), Vector3(0,80,120));
    Quad scene_backWall(Vector3(-190, 20, 700), Vector3(610, 20, 700), Vector3(610, 420, 700), Vector3(-190, 420, 700), Vector3(190,190,190));
    Quad scene_rightWall(Vector3(610, 20, -600), Vector3(610, 420, -600), Vector3(610, 420, 700), Vector3(610, 20, 700), Vector3(190,190,190));
    Quad scene_leftWall(Vector3(-190, 20, -600), Vector3(-190, 20, 700), Vector3(-190, 420, 700), Vector3(-190, 420, -600), Vector3(190,190,190));
    Quad scene_behindWall(Vector3(-190, 420, -600), Vector3(610, 420, -600), Vector3(610, 20, -600), Vector3(-190, 20, -600), Vector3(20,20,20));
    Light lightA(Vector3(150, 300, 200), 10, Vector3(255, 255, 255), 0.4f);
    Light lightB(Vector3(350, 300, 200), 10, Vector3(255, 255, 255), 0.4f);

    pObjectList.push_back(&sphere);
    pObjectList.push_back(&sphereB);
    pObjectList.push_back(&sphereC);
    pObjectList.push_back(&scene_ceiling);
    pObjectList.push_back(&scene_floor);
    pObjectList.push_back(&scene_backWall);
    pObjectList.push_back(&scene_rightWall);
    pObjectList.push_back(&scene_leftWall);
    pObjectList.push_back(&scene_behindWall);
    pObjectList.push_back(&lightA);
    pObjectList.push_back(&lightB);
    pLightList.push_back(&lightA);
    pLightList.push_back(&lightB);

    // Iterate through each pixel on the screen
    for (int i = 0; i < _width; ++ i)
        for (int j = 0; j < _height; ++j)
		{
            // Cast a ray from each pixel
            Pixel px;
            Vector3 PixelPosition((float)j, (float)i, 0);
			Vector3 Direction = Minus(PixelPosition, Camera);
			Direction = Normalize(Direction);

            float t_min = 999999;
            Vector3 Normal_min;
            bool HasIntersection = false;
            Vector3 objectColour(0, 0, 0);
            bool isLight = false;
            bool isReflective = false;

            // Intersect the ray with all objects in the scene
            for (int k = 0; k < pObjectList.size(); ++ k)
            {
                float t;
                Vector3 normal;
                bool DoesIntersect = pObjectList[k]->Intersect(Camera, Direction,
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
                    }
                }
            }

            // If an intersection is found, determine whether to shade the pixel, reflect the ray, or ignore shading (light orbs)
            if (HasIntersection)
            {
                Vector3 Intersection = MultiplyScalar(Direction, t_min);
				Intersection = Add(Intersection, Camera);
                if (isLight)
                    SetColor(px, objectColour);
                else if (isReflective)
                    px = calcReflection(Intersection, Normal_min);
                else
                    px = DiffuseShade(Intersection, Normal_min, objectColour);

            }
            else
			{
                // Otherwise, set the pixel to the background color
				SetColor(px, BackgroundColor);
			}
			 
            (*pImage)(_width-i, j) = px;
        }
}
