#ifndef CUBE_H
#define CUBE_H
#include "Geometry.h"

class Cube : public Geometry
{
public:
    Cube();
    virtual ~Cube();

    virtual void buildGeomtery();

private:
    glm::vec3 center_;
    float sidelength_;
};

#endif