#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glew/glew.h"

#include "Node.h"

Node::Node(Node* parent, Geometry* geometry, std::string name, glm::vec3& translation,
	glm::vec3& rotation, glm::vec3& scale, glm::vec3& center, glm::vec3& color)
	: parent_(parent), geometry_(geometry), name_(name), translation_(translation),
	rotation_(rotation), scale_(scale), center_(center), color_(color)
{
	geometry_->setColor(color);
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
	geometry_->setColor(color_);
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