#include "Octree.h"

OctreeNode::OctreeNode(glm::vec3 &boudingVertexMin, glm::vec3 &boudingVertexMax) : boudingVertexMin_(boudingVertexMin), boudingVertexMax_(boudingVertexMax)
{
	for (unsigned int i=0; i < 8; ++i)
	{
		children_[i] = NULL;
	}
}

void OctreeNode::cleanUp()
{
	for (unsigned int i=0; i < triangleList_.size(); ++i)
	{
		Triangle * tr = triangleList_[i];
		if (tr)
		{
			delete tr;
			tr = NULL;
		}
	}
}

bool OctreeNode::Intersects(Ray r, Intersection * inter)
{
	float boxt;

	if (!intersectsBoudingBox(r))
		return false;
	else
	{
		inter->t = FLT_MAX;

		for (unsigned int i=0; i < 8; ++i)
		{
			bool found = false;

			Intersection childInter;
			if (children_[i] && children_[i]->Intersects(r, &childInter) && childInter.t < inter->t)
			{
				inter->t = childInter.t;
				inter->normal = childInter.normal;
			}
		}
		
		if (inter->t < FLT_MAX)
			return true;

		//We did not intersect a subnode, check for triangles in this node
		unsigned int triangleCount = triangleList_.size();
		Intersection interTriangle;
		interTriangle.t = -1;
		for (unsigned int i = 0; i < triangleCount; ++i)
		{
			if (triangleList_[i]->Intersects(r, &interTriangle) && interTriangle.t < inter->t)
			{
				inter->t = interTriangle.t;
				inter->normal = interTriangle.normal;
			}
		}

		return inter->t < FLT_MAX;
	}
}

OctreeNode* OctreeNode::BuildFromMesh(Mesh *mesh)
{
	// Build a triangle list from the mesh
	std::vector<Triangle*> triangles(mesh->getIndexCount()/3);

	const std::vector<glm::vec3>& vertices = mesh->getVertices();
	const std::vector<unsigned int>& indices = mesh->getIndices();

	for (unsigned int i=0, index=0; i < triangles.size(); ++i)
	{
		triangles[i] = new Triangle(vertices[indices[index]], vertices[indices[index+1]], vertices[indices[index+2]]);
		index += 3;
	}

	OctreeNode* generated = BuildFromMesh(triangles, mesh->boudingBox_[0], mesh->boudingBox_[1], 10); //Make it 9 to 14

	return generated;
}

#include "tribox.cpp"

OctreeNode* OctreeNode::BuildFromMesh(std::vector<Triangle*>& triangles, glm::vec3& boundingMin, glm::vec3& boundingMax, int depth)
{
	if (depth < 0)
		return NULL;
	
	//Divide in 8 sub boxes (divide by two on each dimension)
	glm::vec3 childBoundingMin[8], childBoundingMax[8];
	glm::vec3 boxLength = boundingMax - boundingMin;
	boxLength *= 0.5f;
	glm::vec3 childBoxHalfSize = boxLength*0.5f;

	childBoundingMin[0] = boundingMin;
	childBoundingMin[1] = boundingMin; childBoundingMin[1].x += boxLength.x;
	childBoundingMin[2] = boundingMin; childBoundingMin[2].x += boxLength.x; childBoundingMin[2].y += boxLength.y;
	childBoundingMin[3] = boundingMin; childBoundingMin[3].y += boxLength.y;
	childBoundingMin[4] = childBoundingMin[0]; childBoundingMin[4].z += boxLength.z;
	childBoundingMin[5] = childBoundingMin[1]; childBoundingMin[5].z += boxLength.z;
	childBoundingMin[6] = childBoundingMin[2]; childBoundingMin[6].z += boxLength.z;
	childBoundingMin[7] = childBoundingMin[3]; childBoundingMin[7].z += boxLength.z;

	childBoundingMax[4] = boundingMax; childBoundingMax[4].x -= boxLength.x; childBoundingMax[4].y -= boxLength.y;
	childBoundingMax[5] = boundingMax; childBoundingMax[5].y -= boxLength.y;
	childBoundingMax[6] = boundingMax;
	childBoundingMax[7] = boundingMax; childBoundingMax[7].x -= boxLength.x;
	childBoundingMax[0] = childBoundingMax[4]; childBoundingMax[0].z -= boxLength.z;
	childBoundingMax[1] = childBoundingMax[5]; childBoundingMax[1].z -= boxLength.z;
	childBoundingMax[2] = childBoundingMax[6]; childBoundingMax[2].z -= boxLength.z;
	childBoundingMax[3] = childBoundingMax[7]; childBoundingMax[3].z -= boxLength.z;

	OctreeNode* node = new OctreeNode(boundingMin, boundingMax);
	node->triangleList_ = triangles;
		
	std::vector<Triangle*> subBoxTriangles[8];		

	unsigned int triangleCount = triangles.size();
	if (triangleCount > 3 && depth >= 0)
	{
		for (unsigned int i=0; i < triangleCount; ++i)
		{
			Triangle* tr = triangles[i];
			for (int subbox=0; subbox < 8; ++subbox)
			{
				glm::vec3 boudingCenter = childBoundingMin[subbox] + childBoxHalfSize;

				if ( triBoxOverlap(&boudingCenter[0], &childBoxHalfSize[0], &tr->p1_[0],  &tr->p2_[0], &tr->p3_[0]))
					subBoxTriangles[subbox].push_back( tr );
			}
		}
	
		for (unsigned int i = 0; i < 8; ++i)
		{
			if ( subBoxTriangles[i].size() > 0 )
				node->children_[i] = BuildFromMesh(subBoxTriangles[i], childBoundingMin[i], childBoundingMax[i], depth-1);
		}
	}
	else
	{
		for (unsigned int i = 0; i < 8; ++i)
		{
			node->children_[i] = NULL;
		}
	}

	return node;
}

bool OctreeNode::intersectsBoudingBox(const Ray &ray) const
{
	float a1 = boudingVertexMin_[0] - ray.orig[0];
	float a2 = boudingVertexMax_[0] - ray.orig[0];
	float b1 = boudingVertexMin_[1] - ray.orig[1];
	float b2 = boudingVertexMax_[1] - ray.orig[1];
	float c1 = boudingVertexMin_[2] - ray.orig[2];
	float c2 = boudingVertexMax_[2] - ray.orig[2];

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