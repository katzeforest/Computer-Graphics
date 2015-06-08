#include "Triangle.h"

Triangle::Triangle (const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) : p1_(p1), p2_(p2), p3_(p3)
{
}

bool Triangle::Intersects(Ray ray, Intersection *intersection)
{
	glm::vec3 p1p2(p2_ - p1_);
	glm::vec3 p1p3(p3_ - p1_);

	glm::vec3 P(glm::cross(ray.dir, p1p3));

	float det = glm::dot(p1p2, P);

	if(det > -FLT_EPSILON && det < FLT_EPSILON)
		return false;

	float inv_det = 1.f / det;
 
	glm::vec3 T(ray.orig - p1_);

	float u = glm::dot(T, P) * inv_det;

	if(u < 0.f || u > 1.f)
		return false;
 
	glm::vec3 Q(glm::cross(T, p1p2));
 
	float v = glm::dot(ray.dir, Q) * inv_det;

	if(v < 0.f || u + v  > 1.f)
		return false;
 
	float t = glm::dot(p1p3, Q) * inv_det;
 
	if(t > FLT_EPSILON)
	{
		intersection->t = t;
		intersection->normal = glm::cross(p1p2, p1p3);
		return true;
	}

	return false;
}