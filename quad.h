/*------------------------------
* Quad.h
* Description: defines a quad primitive, which is a plane formed by two triangles.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "object.h"
#include "common.h"
#include "triangle.h"

class Quad : public Object
{
public:

    Quad(Vector3 tA_in, Vector3 tB_in, Vector3 tC_in, Vector3 tD_in, Vector3 colour_in)
    {
        a = Triangle(tA_in, tB_in, tC_in, colour_in);
        b = Triangle(tC_in, tD_in, tA_in, colour_in);
        colour = colour_in;
        isLight = false;
        reflective = false;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction, float * t_out, Vector3 * normal_out)
    {
        if (a.Intersect(Origin, Direction, t_out, normal_out))
            return true;
        if (b.Intersect(Origin, Direction, t_out, normal_out))
            return true;
        return false;
    }

    Triangle a, b;
};
