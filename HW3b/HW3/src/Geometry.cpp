#include "Geometry.h"
#include "EasyBMP.h"
#include <glm/glm.hpp>

Geometry::Geometry(geometryType geomType) :
    type_(geomType)
{
}

Geometry::~Geometry()
{
    vertices_.clear();
    normals_.clear();
    colors_.clear();
    indices_.clear();
}

Intersection Geometry::intersect(const glm::mat4 &T, Ray ray_world) const
{
    // The input ray here is in WORLD-space. It may not be normalized!

    // TODO: normalize ray_world.
	ray_world.dir = glm::normalize(ray_world.dir);

    // Transform the ray into OBJECT-LOCAL-space, for intersection calculation.
    Ray ray_local;// TODO: COMPUTE THIS
    // TODO: transform the ray by the inverse transformation.
    //     (Remember that position = vec4(vec3, 1) while direction = vec4(vec3, 0).)
	glm::vec4 position = glm::vec4(ray_world.orig, 1);
	glm::vec4 direction = glm::vec4(ray_world.dir, 0);

	glm::mat4 inverseT = glm::inverse(T);

	ray_local.orig = glm::vec3(inverseT * position);
	ray_local.dir = glm::vec3(inverseT * direction);

    // Compute the intersection in LOCAL-space.
    Intersection isx = intersectImpl(ray_local);

    if (isx.t != -1) {
        // Transform the local-space intersection BACK into world-space.
        //     (Note that, as long as you didn't re-normalize the ray direction
        //     earlier, `t` doesn't need to change.)
        const glm::vec3 normal_local = isx.normal;
        glm::vec3 normal_world;  // TODO: COMPUTE THIS
        // TODO: inverse-transpose-transform the normal to get it back from local-space to world-space.
		glm::mat4 transposed = glm::transpose(inverseT);

		normal_world = glm::vec3(transposed * glm::vec4(normal_local,0));
        //     (If you were transforming a position, you would just use the unmodified transform T.)
        //     http://www.arcsynthesis.org/gltut/Illumination/Tut09%20Normal%20Transformation.html
        isx.normal = normal_world;
    }

    // The final output intersection data is in WORLD-space.
    return isx;
}

bool Geometry::intersectTriangle(const Ray &ray, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3, Intersection *intersection, glm::vec3 const* n1, glm::vec3 const* n2, glm::vec3 const* n3)
{
	glm::vec3 p1p2(p2 - p1);
	glm::vec3 p1p3(p3 - p1);

	glm::vec3 P(glm::cross(ray.dir, p1p3));

	float det = glm::dot(p1p2, P);

	if(det > -FLT_EPSILON && det < FLT_EPSILON)
		return false;

	float inv_det = 1.f / det;
 
	glm::vec3 T(ray.orig - p1);

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

		if (n1 != NULL)
		{
			glm::vec3 P(ray.orig + ray.dir * t);
			glm::vec3 p1pi(P - p1);
			glm::vec3 p2pi(P - p2);
			glm::vec3 p3pi(P - p3);
			float p1piLen = glm::length(p1pi), p2piLen = glm::length(p2pi), p3piLen = glm::length(p3pi);
			float totalDistance = (p1piLen + p2piLen + p3piLen);
			intersection->normal = glm::vec3((totalDistance-p1piLen)/totalDistance * glm::normalize(*n1) + (totalDistance-p2piLen)/totalDistance * glm::normalize(*n2) + (totalDistance-p3piLen)/totalDistance * glm::normalize(*n3));
		}
		else
			intersection->normal = glm::cross(p1p2, p1p3);

		return true;
	}

	return false;
}