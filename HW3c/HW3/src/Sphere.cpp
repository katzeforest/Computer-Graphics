#include "Sphere.h"

static const float PI = 3.141592653589f;

// Creates a unit sphere.
Sphere::Sphere() :
    Geometry(SPHERE),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(1.0f)
{
    buildGeomtery();
}

Sphere::~Sphere() {}

void Sphere::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    // Find vertex positions for the sphere.
    unsigned int subdiv_axis = 16;      // vertical slices
    unsigned int subdiv_height = 16;        // horizontal slices
    float dphi = PI / subdiv_height;
    float dtheta = 2.0f * PI / subdiv_axis;
    float epsilon = 0.0001f;
    glm::vec3 color (0.6f, 0.6f, 0.6f);

    // North pole
    glm::vec3 point (0.0f, 1.0f, 0.0f);
    normals_.push_back(point);
    // scale by radius_ and translate by center_
    vertices_.push_back(center_ + radius_ * point);

    for (float phi = dphi; phi < PI; phi += dphi) {
        for (float theta = dtheta; theta <= 2.0f * PI + epsilon; theta += dtheta) {
            float sin_phi = sin(phi);

            point[0] = sin_phi * sin(theta);
            point[1] = cos(phi);
            point[2] = sin_phi * cos(theta);

            normals_.push_back(point);
            vertices_.push_back(center_ + radius_ * point);
        }
    }
    // South pole
    point = glm::vec3(0.0f, -1.0f, 0.0f);
    normals_.push_back(point);
    vertices_.push_back(center_ + radius_ * point);

    // fill in index array.
    // top cap
    for (unsigned int i = 0; i < subdiv_axis - 1; ++i) {
        indices_.push_back(0);
        indices_.push_back(i + 1);
        indices_.push_back(i + 2);
    }
    indices_.push_back(0);
    indices_.push_back(subdiv_axis);
    indices_.push_back(1);

    // middle subdivs
    unsigned int index = 1;
    for (unsigned int i = 0; i < subdiv_height - 2; i++) {
        for (unsigned int j = 0; j < subdiv_axis - 1; j++) {
            // first triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis);
            indices_.push_back(index + subdiv_axis + 1);

            // second triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis + 1);
            indices_.push_back(index + 1);

            index++;
        }
        // reuse vertices from start and end point of subdiv_axis slice
        indices_.push_back(index);
        indices_.push_back(index + subdiv_axis);
        indices_.push_back(index + 1);

        indices_.push_back(index);
        indices_.push_back(index + 1);
        indices_.push_back(index + 1 - subdiv_axis);

        index++;
    }

    // end cap
    unsigned int bottom = (subdiv_height - 1) * subdiv_axis + 1;
    unsigned int offset = bottom - subdiv_axis;
    for (unsigned int i = 0; i < subdiv_axis - 1 ; ++i) {
        indices_.push_back(bottom);
        indices_.push_back(i + offset);
        indices_.push_back(i + offset + 1);
    }
    indices_.push_back(bottom);
    indices_.push_back(bottom - 1);
    indices_.push_back(offset);

    // colors
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        colors_.push_back(glm::vec3(0.6, 0.6, 0.6));
    }
}

#include <iostream>

Intersection Sphere::intersectImpl(const Ray &ray) const
{
	Intersection inter;

	float t1 = -1;
	float t2 = -1;

	inter.t = -1;

	glm::vec3 temporary = ray.orig - center_;

	float b = 2 * glm::dot(ray.dir, temporary);
	float a = glm::dot(ray.dir, ray.dir);
	float c = glm::dot(temporary, temporary) - (radius_ * radius_);

	float disc = b*b - 4*a*c;

	if(disc >= 0)
	{
		float discr = sqrt(disc);
		t1 = (-b+discr)/(2*a); 
		t2 = (-b-discr)/(2*a);   
		if(t1 > 0 && t2 > 0)
			inter.t = std::min(t1, t2);
		else if(t1 > 0)
			inter.t = t1;
		else if(t2 > 0)
			inter.t = t2;
	}

	if (inter.t > 0)
	{
		//std::cout << inter.t << std::endl;
		glm::vec3 P = ray.orig + ray.dir * float(inter.t);
		inter.normal = glm::normalize(P - center_);
	}

	return inter;

}
/*
template <typename T>
bool solveQuadratic(const T &a, const T &b, const T &c, T &x0, T &x1)
{
	T discr = b * b - 4 * a * c;
	if (discr < 0) 
		return false;
	else if (discr == 0) 
		x0 = x1 = - 0.5 * b / a;
	else
	{
		T q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
		x0 = q / a; 
		x1 = c / q;
	}

	if (x0 > x1) 
		std::swap(x0, x1); 
	return true;
}*/