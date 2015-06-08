#pragma once
#include "Geometry.h"

class Mesh : public Geometry
{
public:
    Mesh(const std::string &file);
    virtual ~Mesh();

    virtual void buildGeomtery();

	void loadObjFile(const std::string &file);
	
protected:
	virtual Intersection intersectImpl(const Ray &ray) const;

private:
	string objFile_;
    glm::vec3 center_;
    float sidelength_;
	glm::vec3 boudingBox_[2];

	bool intersectsHitBox(const Ray &ray) const;
};