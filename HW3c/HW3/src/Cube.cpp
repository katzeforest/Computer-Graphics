#include "Cube.h"
#include "Config.h"
#include "EasyBMP.h"
#include <glm/glm.hpp>
#include "glew/glew.h"


// Creates a unit cube.
Cube::Cube() :
    Geometry(CUBE),
    center_(glm::vec3(0.f, 0.f, -1.5f)),
    sidelength_(1.0f)
{
    buildGeomtery();
}

Cube::~Cube() {}

void Cube::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();
	
    // cube vertices
		// front
	vertices_.push_back(glm::vec3(-0.5, 0.5, 0.5));
	vertices_.push_back(glm::vec3(-0.5, -0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, 0.5, 0.5));
	
	// back
	vertices_.push_back(glm::vec3(-0.5, 0.5, -0.5));
	vertices_.push_back(glm::vec3(0.5, 0.5, -0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, -0.5));
	vertices_.push_back(glm::vec3(-0.5,-0.5,-0.5));

	//right side
	vertices_.push_back(glm::vec3(0.5, 0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, -0.5));
	vertices_.push_back(glm::vec3(0.5, 0.5, -0.5));

	//left side
	vertices_.push_back(glm::vec3(-0.5, 0.5, -0.5));
	vertices_.push_back(glm::vec3(-0.5,-0.5,-0.5));
	vertices_.push_back(glm::vec3(-0.5, -0.5, 0.5));
	vertices_.push_back(glm::vec3(-0.5, 0.5, 0.5));

	//top
	vertices_.push_back(glm::vec3(-0.5, 0.5, -0.5));
	vertices_.push_back(glm::vec3(-0.5, 0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, 0.5, 0.5));
	vertices_.push_back(glm::vec3(0.5, 0.5, -0.5));

	//bottom
	vertices_.push_back(glm::vec3(-0.5, -0.5, 0.5));
	vertices_.push_back(glm::vec3(-0.5,-0.5,-0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, -0.5));
	vertices_.push_back(glm::vec3(0.5, -0.5, 0.5));
	
	// cube Normals
	// front
	normals_.push_back(glm::vec3(0, 0, 1));
	normals_.push_back(glm::vec3(0, 0, 1));
	normals_.push_back(glm::vec3(0, 0, 1));
	normals_.push_back(glm::vec3(0, 0, 1));
	
	// back
	normals_.push_back(glm::vec3(0, 0, -1));
	normals_.push_back(glm::vec3(0, 0, -1));
	normals_.push_back(glm::vec3(0, 0, -1));
	normals_.push_back(glm::vec3(0, 0, -1));

	//right side
	normals_.push_back(glm::vec3(1, 0, 0));
	normals_.push_back(glm::vec3(1, 0, 0));
	normals_.push_back(glm::vec3(1, 0, 0));
	normals_.push_back(glm::vec3(1, 0, 0));

	//left side
	normals_.push_back(glm::vec3(-1, 0, 0));
	normals_.push_back(glm::vec3(-1, 0, 0));
	normals_.push_back(glm::vec3(-1, 0, 0));
	normals_.push_back(glm::vec3(-1, 0, 0));

	//top
	normals_.push_back(glm::vec3(0, 1, 0));
	normals_.push_back(glm::vec3(0, 1, 0));
	normals_.push_back(glm::vec3(0, 1, 0));
	normals_.push_back(glm::vec3(0, 1, 0));

	//bottom
	normals_.push_back(glm::vec3(0, -1, 0));
	normals_.push_back(glm::vec3(0, -1, 0));
	normals_.push_back(glm::vec3(0, -1, 0));
	normals_.push_back(glm::vec3(0, -1, 0));
	
	unsigned int vertices = vertices_.size();
	for (unsigned int i=0; i<vertices; ++i)
	{
		colors_.push_back(glm::vec3(0.6f, 0.6f, 0.6f));
	}
	
	for(unsigned int i=0; i<6; ++i)
	{
		indices_.push_back(i*4);
		indices_.push_back(i*4+1);
		indices_.push_back(i*4+2);

		indices_.push_back(i*4);
		indices_.push_back(i*4+2);
		indices_.push_back(i*4+3);
	}
}

Intersection Cube::intersectImpl(const Ray &ray) const
{
	// TODO REPLACE BY CORRECT CODE

	Intersection inter;

	glm::vec3 min(-0.5, -0.5, -0.5);
	glm::vec3 max( 0.5,  0.5,  0.5);

	//m_position is the camera position
	float a1 = min[0] - ray.orig[0];
	float a2 = max[0] - ray.orig[0];
	float b1 = min[1] - ray.orig[1];
	float b2 = max[1] - ray.orig[1];
	float c1 = min[2] - ray.orig[2];
	float c2 = max[2] - ray.orig[2];

	float t1x, t2x, t1y, t2y, t1z, t2z;


	t1x =  a1 / ray.dir[0];
	t2x =  a2 / ray.dir[0];
    t1y =  b1 / ray.dir[1];
    t2y =  b2 / ray.dir[1];
    t1z =  c1 / ray.dir[2];
	t2z =  c2 / ray.dir[2];

	float Tnear = -DBL_MAX , Tfar = DBL_MAX;

	//X direction
	if(t1x > t2x) {std::swap(t1x, t2x);}
	if(Tnear < t1x) {Tnear = t1x;}
	if(Tfar > t2x)  {Tfar = t2x;}

	//Y direction
	if(t1y > t2y) {std::swap(t1y, t2y);}
	if(Tnear < t1y) {Tnear = t1y;}
	if(Tfar > t2y)  {Tfar = t2y;}

	//Z direction
	if(t1z > t2z) {std::swap(t1z, t2z);}
	if(Tnear < t1z) {Tnear = t1z;}
	if(Tfar > t2z)  {Tfar = t2z;}


	static const float EPSILON = 1E-3f;

	if(Tnear < Tfar) //true
	{
		glm::vec3 pNear = ray.orig + ray.dir * Tnear;
		inter.t = Tnear;

		if (abs(pNear.x - 0.5f) < EPSILON)
			inter.normal = glm::vec3(1, 0, 0);
		else if (abs(pNear.x + 0.5f) < EPSILON)
			inter.normal = glm::vec3(-1, 0, 0);
		else if (abs(pNear.y - 0.5f) < EPSILON)
			inter.normal = glm::vec3(0, 1, 0);
		else if (abs(pNear.y + 0.5f) < EPSILON)
			inter.normal = glm::vec3(0, -1, 0);
		else if (abs(pNear.z - 0.5f) < EPSILON)
			inter.normal = glm::vec3(0, 0, 1);
		else if (abs(pNear.z + 0.5f) < EPSILON)
			inter.normal = glm::vec3(0, 0, -1);
		else
		{
			//std::cout << "Failed to find a normal" << std::endl;
			inter.normal = glm::vec3(0,0,0);
		}
	}
	else
		inter.t = -1;
		
	return inter;
}