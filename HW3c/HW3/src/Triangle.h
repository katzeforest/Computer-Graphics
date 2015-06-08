#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Ray.h"
#include "Geometry.h"

class Triangle
{
public:
	Triangle (const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);

	bool Intersects(Ray r, Intersection *intersection);

	glm::vec3 p1_, p2_, p3_;
};