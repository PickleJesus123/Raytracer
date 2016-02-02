/*------------------------------
* Object.h
* Description: a parent class that is present in every object rendered in the scene.
*
* By: Harrison Walters
-------------------------------*/

#pragma once
#include "common.h"

class Object
{
public:
    virtual bool Intersect(Vector3 Origin, Vector3 Direction, float * t, Vector3 * normal) = 0;

    Vector3 colour;
    bool isLight;
    bool reflective;
};
