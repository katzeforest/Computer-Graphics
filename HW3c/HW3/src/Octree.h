#pragma once

#include <vector>
#include "Ray.h"
#include "Mesh.h"
#include "Triangle.h"

class OctreeNode
{
public:
	static OctreeNode* OctreeNode::BuildFromMesh(Mesh *mesh);
	void cleanUp();

	bool Intersects(Ray r, Intersection * inter);
	bool intersectsBoudingBox(const Ray &ray) const;

private:
	OctreeNode::OctreeNode(glm::vec3 &boudingVertexMin, glm::vec3 &boudingVertexMax);
	glm::vec3 boudingVertexMin_, boudingVertexMax_;
	std::vector<Triangle*> triangleList_;
	OctreeNode * children_[8];

	static OctreeNode* BuildFromMesh(std::vector<Triangle*>& triangles, glm::vec3& boundingMin, glm::vec3& boundingMax, int depth);

};