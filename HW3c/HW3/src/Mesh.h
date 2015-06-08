#pragma once
#include "Geometry.h"

class OctreeNode;

class Mesh : public Geometry
{
public:
    Mesh(const std::string &file);
    virtual ~Mesh();

    virtual void buildGeomtery();

	void loadObjFile(const std::string &file);

	glm::vec3 boudingBox_[2];
	
protected:
	virtual Intersection intersectImpl(const Ray &ray) const;

private:
	string objFile_;
    glm::vec3 center_;
    float sidelength_;

	OctreeNode * octree_;

	bool intersectsHitBox(const Ray &ray) const;
};