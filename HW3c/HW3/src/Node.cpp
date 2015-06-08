#define _USE_MATH_DEFINES
#include <cmath> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glew/glew.h"

#include "Node.h"

Node::Node(Node* parent, Geometry* geometry, std::string name, glm::vec3& translation,
	glm::vec3& rotation, glm::vec3& scale, glm::vec3& center, Material* material)
	: parent_(parent), geometry_(geometry), name_(name), translation_(translation),
	rotation_(rotation), scale_(scale), center_(center), material_(material)
{
	resetColor();
}

Node::~Node() 
{
	for (unsigned int i=0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child)
			delete child;
		children_[i] = NULL;
	}
}

void Node::addChild(Node* child)
{
	children_.push_back(child);
}

Node * Node::findChildByName(std::string& name)
{
	if (name.compare(name_) == 0)
		return this;

	for (unsigned int i=0; i < children_.size(); ++i)
	{
		Node * found = children_[i]->findChildByName(name);
		if (found)
			return found;
	}
	return NULL;
}

void Node::deleteAllChildren()
{
	for (unsigned int i = 0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child != NULL)
			delete child;
	}

	children_.clear();
}

void Node::deleteChild(Node * childParam)
{
	for (unsigned int i = 0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child == childParam)
		{
			delete child;
			children_.erase(children_.begin() + i);
			return;
		}
	}
}

void Node::setColor(glm::vec3 color)
{
	geometry_->setColor(color);
}

void Node::resetColor()
{
	if (material_)
		geometry_->setColor(material_->getDiffuseColor());
}

void Node::buildArrays(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<glm::vec3> &colors, std::vector<GLuint> &indices)
{
	if (geometry_)
	{
		unsigned int offset = positions.size();
		positions.insert(positions.end(), geometry_->getVertices().begin(), geometry_->getVertices().end());
		normals.insert(normals.end(), geometry_->getNormals().begin(), geometry_->getNormals().end());
		colors.insert(colors.end(), geometry_->getColors().begin(), geometry_->getColors().end());

		std::vector<GLuint> shiftedIndices(geometry_->getIndices());
		unsigned int indicesSize = shiftedIndices.size();
		for(unsigned int i = 0; i < indicesSize; ++i)
			shiftedIndices[i] += offset;
		indices.insert(indices.end(), shiftedIndices.begin(), shiftedIndices.end());
	}

	for(unsigned int i=0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child)
			child->buildArrays(positions, normals, colors, indices);
	}
}

void Node::drawScene(glm::mat4 model, unsigned int unifModel, unsigned int unifModelInvTr, unsigned int &bufferOffset)
{

	if (geometry_)
	{
		// TODO use the right transformation center

		// Rotate
		model = glm::rotate(model, rotation_[0], glm::vec3(1, 0, 0));
		model = glm::rotate(model, rotation_[1], glm::vec3(0, 1, 0));
		model = glm::rotate(model, rotation_[2], glm::vec3(0, 0, 1));

		// Translate
		model = glm::translate(model, translation_);

		// Scale
		model = glm::scale(model, scale_);
		
		// Set the 4x4 model transformation matrices
		// Pointer to the first element of the array
		glUniformMatrix4fv(unifModel, 1, GL_FALSE, &model[0][0]);
		// Also upload the inverse transpose for normal transformation
		const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(model));
		glUniformMatrix4fv(unifModelInvTr, 1, GL_FALSE, &modelInvTranspose[0][0]);

		glDrawElements(GL_TRIANGLES, geometry_->getIndexCount(), GL_UNSIGNED_INT, (void*) (sizeof(GLuint) * bufferOffset));
		bufferOffset += geometry_->getIndexCount();
	}

	for(unsigned int i=0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child)
			child->drawScene(model, unifModel, unifModelInvTr, bufferOffset);
	}
}

#include <iostream>

Intersection Node::intersect(glm::mat4 T, Ray ray_world)
{
	// Rotate
	T = glm::rotate(T, rotation_[0], glm::vec3(1, 0, 0));
	T = glm::rotate(T, rotation_[1], glm::vec3(0, 1, 0));
	T = glm::rotate(T, rotation_[2], glm::vec3(0, 0, 1));

	// Translate
	T = glm::translate(T, translation_);

	// Scale
	T = glm::scale(T, scale_);

	Intersection interThisNode;
	interThisNode.t = DBL_MAX;
	
	bool hit=false;

	for(unsigned int i=0; i < children_.size(); ++i)
	{
		Node * child = children_[i];
		if (child)
		{
			Intersection interChild = child->intersect(T, ray_world);
			if ( interChild.t >= 0 && interChild.t < interThisNode.t )
			{
				hit = true;
				interThisNode.t = interChild.t;
				interThisNode.normal = interChild.normal;
				interThisNode.intersectionPoint = interChild.intersectionPoint;
				interThisNode.intersectedObject = interChild.intersectedObject;
			}
		}
	}

	Intersection interThisGeometry = geometry_->intersect(T, ray_world);
	if ( interThisGeometry.t >= 0 && interThisGeometry.t < interThisNode.t)
	{
		hit = true;
		interThisNode.t = interThisGeometry.t;
		interThisNode.normal = interThisGeometry.normal;
		interThisNode.intersectionPoint = ray_world.orig + ray_world.dir * float(interThisGeometry.t);
		interThisNode.intersectedObject = this;
	}

	if (!hit)
	{
		interThisNode.t = -1;
		interThisNode.intersectedObject = NULL;
	}

	return interThisNode;
}

#include "sampling.cpp"

glm::vec3 Node::getRandomPoint()
{
	if (geometry_)
	{
		switch (geometry_->getGeometryType())
		{
		case Geometry::CUBE: return getRandomPointOnCube(this);
		case Geometry::SPHERE: return getRandomPointOnSphere(this);
		}
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}