/*------------------------------
* Sphere.h
* Description: defines a sphere primitive, and allows it to be rendered in the scene.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "object.h"

class Sphere : public Object
{
public:

    Sphere() {
        
    }

    Sphere(Vector3 Center_in, float Radius_in, Vector3 colour_in, bool reflective_in)
    {
        Center = Center_in;
        Radius = Radius_in;
        colour = colour_in;
        isLight = false;
        reflective = reflective_in;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float *t_out, Vector3 *normal_out)
    {
        Vector3 EO = Minus(Center, Origin);
        float v = DotProduct(EO, Direction);
        float RadiusSquare = Radius * Radius;
        float EO_Square = DotProduct(EO, EO);
        float discriminant = RadiusSquare - (EO_Square - v * v);

        float t = -1;
        if (discriminant > 0)
        {
            float d = sqrt(discriminant);
            t = v - d;
        }

        if (t > 0) {
            *t_out = t;
            Vector3 IntersectionPoint = MultiplyScalar(Direction, t);
            IntersectionPoint = Add(IntersectionPoint, Origin);
            Vector3 SurfaceNormal = Minus(IntersectionPoint, Center);
            (*normal_out) = Normalize(SurfaceNormal);
            return true;
        } else
        {
            return false;
        }
    }

    Vector3 Center;
    float Radius;
};
