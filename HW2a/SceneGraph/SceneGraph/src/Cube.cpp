#include "Cube.h"

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