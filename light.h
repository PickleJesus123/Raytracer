/*------------------------------
* Light.h
*
* Description:
* Casts light into the scene when iterated over in the raytracer script.
* This type does not cast or recieve shadows, and appears as a glowing orb of the chosen color.
* You can adjust the intensity of the light emitted by the orb, and change its sphere size.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "object.h"
#include "common.h"
#include "sphere.h"

class Light : public Object
{
public:

    Light(Vector3 origin_in, float radius_in, Vector3 colour_in, float intensity_in)
    {
        origin = origin_in;
        bulb = Sphere(origin_in, radius_in, colour_in, false);
        colour = colour_in;
        isLight = true;
        intensity = intensity_in;
        reflective = false;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction, float * t_out, Vector3 * normal_out)
    {
        return bulb.Intersect(Origin, Direction, t_out, normal_out);
    }

    Vector3 origin;
    float intensity;
    Sphere bulb;
};
