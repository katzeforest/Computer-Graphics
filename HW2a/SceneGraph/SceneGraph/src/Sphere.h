#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry
{
public:
    Sphere();
    virtual ~Sphere();

    virtual void buildGeomtery();

private:
    glm::vec3 center_;
    float radius_;
};

#endif