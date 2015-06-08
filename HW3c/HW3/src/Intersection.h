#ifndef INTERSECTION_H
#define INTERSECTION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Node;

struct Intersection {
    // The parameter `t` along the ray which was used. (A negative value indicates no intersection.)
    double t;
    // The surface normal at the point of intersection. (Ignored if t < 0.)
    glm::vec3 normal;

	glm::vec3 intersectionPoint; // Coordinates of the interseciton point in global coordinates
	Node * intersectedObject; // A pointer to the intersected object
};

#endif
