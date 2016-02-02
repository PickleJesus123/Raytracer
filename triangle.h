/*------------------------------
* Triangle.h
* Description: defines a triangle primitive, which consists of 3 vertices in the scene.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "object.h"
#include "common.h"

class Triangle : public Object
{
public:

    Triangle() {

    }

    Triangle(Vector3 tA_in, Vector3 tB_in, Vector3 tC_in, Vector3 colour_in)
    {
        tA = tA_in;
        tB = tB_in;
        tC = tC_in;
        colour = colour_in;
        isLight = false;
        reflective = false;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal_out)
    {
        Vector3 normal = Normalize(CrossProduct(Minus(tB,tA),Minus(tB,tC)));
        float t = DotProduct(normal, Minus(tB, Origin)) / DotProduct(normal, Direction);
        Vector3 intersect = Add(Origin, MultiplyScalar(Direction, t));

        bool signA = sign(DotProduct(CrossProduct(Minus(tB, tA),Minus(intersect, tA)),normal));
        bool signB = sign(DotProduct(CrossProduct(Minus(tC, tB),Minus(intersect, tB)),normal));
        bool signC = sign(DotProduct(CrossProduct(Minus(tA, tC),Minus(intersect, tC)),normal));

        *normal_out = normal;
        *t_out = t;

        return ((signA == signB) && (signA == signC) & t>0.1);
    }

    bool sign(int input) {
        if (input > 0.1f)
            return true;
        else
            return false;
    }

    Vector3 tA, tB, tC;
};
