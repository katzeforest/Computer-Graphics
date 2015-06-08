#include "Mesh.h"
#include <iostream>
#include <fstream>
#include "Octree.h"

const unsigned int BUFFERSIZE=512;

// Creates a unit cube.
Mesh::Mesh(const std::string &file) :
	Geometry(MESH)
{
	octree_ = NULL;

	boudingBox_[0] = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	boudingBox_[1] = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
	loadObjFile(file);

#ifdef _OCTREE
	octree_ = OctreeNode::BuildFromMesh(this);
#endif

	//Fill colors
	unsigned int verticesCount = vertices_.size();
	for (unsigned int i=0; i < verticesCount; ++i)
		colors_.push_back(glm::vec3(0.5,0.5,0.5));
}

Mesh::~Mesh()
{
#ifdef _OCTREE
	if (octree_)
	{
		octree_->cleanUp();
		delete octree_;
	}
#endif
}

void Mesh::loadObjFile(const std::string &file)
{
	std::ifstream in(file.c_str(), std::ifstream::in);

	if(!in.is_open())  
	{  
		std::cerr  << "ERROR: Could not open file: " << file.c_str() << std::endl;  
		return;  
	}  
  
	char buffer[BUFFERSIZE];
	unsigned short* dividers = NULL;
	//std::vector<unsigned short> dividers;
      
	while(!in.getline(buffer, BUFFERSIZE-1).eof())  
	{  
		buffer[BUFFERSIZE-1]='\0';
		
		// vertex
		if (buffer[0]=='v' && buffer[1]==' ')  
		{
			float x, y, z;
			if ( sscanf(buffer,"v %f %f %f",&x,&y,&z)==3)  
			{  
				glm::vec3 v = glm::vec3(x,y,z);
				vertices_.push_back(v);
				
				if (boudingBox_[0].x > v.x)
					boudingBox_[0].x = v.x;
				if (boudingBox_[0].y > v.y)
					boudingBox_[0].y = v.y;
				if (boudingBox_[0].z > v.z)
					boudingBox_[0].z = v.z;

				if (boudingBox_[1].x < v.x)
					boudingBox_[1].x = v.x;
				if (boudingBox_[1].y < v.y)
					boudingBox_[1].y = v.y;
				if (boudingBox_[1].z < v.z)
					boudingBox_[1].z = v.z;
			}  
			else  
			{  
				std::cerr << "Cannot read vertex: " << buffer << std::endl;  
				return;  
			}  
		}  
		// face
		else if (buffer[0]=='f' && buffer[1]==' ')  
		{
			// At this point we finished reading vertices
			if (dividers == NULL) // Not initialized yet
			{
				unsigned int verticesSize = vertices_.size();
				dividers = new unsigned short[verticesSize];
				normals_.resize(verticesSize, glm::vec3(0,0,0));
				memset(dividers, 0, verticesSize*sizeof(unsigned short));
			}

			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3;
			unsigned int n1, n2, n3;
			bool normalsRead = false;

			if (sscanf(buffer,"f %u/%u/%u %u/%u/%u %u/%u/%u",&v1,&vt1,&n1,&v2,&vt2,&n2,&v3,&vt3,&n3) == 9)
				normalsRead = true;
			else if (sscanf(buffer,"f %u//%u %u//%u %u//%u",&v1,&n1,&v2,&n2,&v3,&n3) == 6)
				normalsRead = true;
			else if (sscanf(buffer,"f %u/%u %u/%u %u/%u",&v1,&vt1,&v2,&vt2,&v3,&vt3) == 6) ;
			else
			{
				std::cerr << cout << "Unknown face format: " << buffer << std::endl;  
				return;  
			}

			// adjust indices
			v1--; v2--; v3--;
			
			//compute normals
			glm::vec3 n;

			if (normalsRead)
				n = glm::normalize(glm::vec3(n1,n2,n3));
			else
			{
				glm::vec3 u = vertices_[v1] - vertices_[v3];
				glm::vec3 v = vertices_[v2] - vertices_[v3];
				n = glm::normalize(glm::cross(u, v));
			}

			indices_.push_back(v1);
			normals_[v1] += n;
			dividers[v1]++;

			indices_.push_back(v2);
			normals_[v2] += n;
			dividers[v2]++;

			indices_.push_back(v3);
			normals_[v3] += n;
			dividers[v3]++;
		}  
	}  

	// compute normal averages
	unsigned int verticesSize = vertices_.size();
	for (unsigned int i = 0; i < verticesSize; ++i)
	{
		if (dividers[i] != 0)
		{
			float multiple = 1.0f/dividers[i];
			normals_[i] = glm::normalize(normals_[i] * multiple);
		}
	}

	// Make bouding box a bit bigger
	static const float EPS = 1E-6;
	boudingBox_[0].x -= EPS; 
	boudingBox_[0].y -= EPS; 
	boudingBox_[0].z -= EPS; 
	boudingBox_[1].x += EPS; 
	boudingBox_[1].y += EPS; 
	boudingBox_[1].z += EPS; 

	std::cout << "===== LOADED OBJ FILE =====" << std::endl;
	std::cout << vertices_.size() << " vertices, " << indices_.size()/3 << " triangles." << std::endl;
	std::cout << "Bouding vertices (" << boudingBox_[0].x << ", " << boudingBox_[0].y << ", " << boudingBox_[0].z << ") " << ", (" << boudingBox_[1].x << ", " << boudingBox_[1].y << ", " << boudingBox_[1].z << ") " << std::endl;
}

void Mesh::buildGeomtery()
{}

bool Mesh::intersectsHitBox(const Ray &ray) const
{
	float a1 = boudingBox_[0][0] - ray.orig[0];
	float a2 = boudingBox_[1][0] - ray.orig[0];
	float b1 = boudingBox_[0][1] - ray.orig[1];
	float b2 = boudingBox_[1][1] - ray.orig[1];
	float c1 = boudingBox_[0][2] - ray.orig[2];
	float c2 = boudingBox_[1][2] - ray.orig[2];

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

	return (Tnear < Tfar);
}

Intersection Mesh::intersectImpl(const Ray &ray) const
{
	// TODO REPLACE BY CORRECT CODE
	Intersection inter;

	
#ifdef _OCTREE

	if (!octree_->Intersects(ray, &inter))
		inter.t = -1;

#else

	// First do a raybox intersection with the hitbox
	if (!intersectsHitBox(ray))
	{
		inter.t = -1;
		return inter;
	}

	inter.t = DBL_MAX;

	// if hit then check each face...
	unsigned int indiceSize = indices_.size() - 2;

	bool hit = false;
	for (unsigned int i = 0; i < indiceSize; i+=3)
	{
		Intersection interTmp;
		interTmp.t = -1;
		if (Geometry::intersectTriangle(ray, vertices_[indices_[i]], vertices_[indices_[i+1]], vertices_[indices_[i+2]], &interTmp/*, &normals_[indices_[i]], &normals_[indices_[i+1]], &normals_[indices_[i+2]]*/))
		{
			if (interTmp.t >=0 && interTmp.t < inter.t)
			{
				hit = true;
				inter.t = interTmp.t;
				inter.normal = interTmp.normal;
			}
		}
	}

	if (!hit)
		inter.t = -1;
#endif

	return inter;
}