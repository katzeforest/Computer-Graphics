#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glew/glew.h"

#include "Scene.h"

Scene::Scene()
{
	root_ = NULL;
}

Scene::~Scene() {
	if (root_)
		delete root_;
	root_ = NULL;
}

void Scene::setRoot(Node * root)
{
	root_ = root;
}

Node * Scene::getRoot()
{
	return root_;
}

Node * Scene::getNodeFromName(std::string& name)
{
	if (root_ != NULL)
		return root_->findChildByName(name);
	else
		return NULL;
}

Node * Scene::getNextNode(Node * start)
{
	if (start->getChildrenCount() > 0)
		return start->getChild(0);
	else
	{
		Node * currentNode = start;
		while (currentNode != root_)
		{
			//Find if there is a next child for parent
			Node * parentNode = currentNode->getParent();
			for (unsigned int i=0; i < parentNode->getChildrenCount(); ++i)
			{
				Node * child = parentNode->getChild(i);
				if (child == currentNode)
				{
					if (i+1 < parentNode->getChildrenCount())
					{
						//Move to next child
						return parentNode->getChild(i+1);
					}
				}
			}

			//else we go parent node
			currentNode = parentNode;
		}

		return root_; 
	}
}

void Scene::uploadToBuffers(unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx)
{
	static const GLsizei SIZE_POS = sizeof(glm::vec3);
	static const GLsizei SIZE_NOR = sizeof(glm::vec3);
	static const GLsizei SIZE_COL = sizeof(glm::vec3);
	static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

	positions_.clear();
	normals_.clear();
	colors_.clear();
	indices_.clear();

	if (root_)
	{
		root_->buildArrays(positions_, normals_, colors_, indices_);
	}

	if (positions_.size() == 0)
		return;

	// ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
	// Now we put the data into the Vertex Buffer Object for the graphics system to use
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	// Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
	// Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
	// Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &positions_[0], GL_STATIC_DRAW);

	// Bind+upload the color data
	glBindBuffer(GL_ARRAY_BUFFER, vboCol);
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &colors_[0], GL_STATIC_DRAW);

	// Bind+upload the normals
	glBindBuffer(GL_ARRAY_BUFFER, vboNor);
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &normals_[0], GL_STATIC_DRAW);

	// Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 3 * SIZE_TRI, &indices_[0], GL_STATIC_DRAW);
}

void Scene::drawScene(glm::mat4 model, unsigned int locationPos, unsigned int locationCol, unsigned int locationNor,
	unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx, unsigned int unifModel, unsigned int unifModelInvTr)
{
	if (root_)
	{
		// =============================== Draw the data that we sent =================================
		// Activate our three kinds of vertex information
		glEnableVertexAttribArray(locationPos);
		glEnableVertexAttribArray(locationCol);
		glEnableVertexAttribArray(locationNor);

		// Tell the GPU where the positions are: in the position buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboPos);
		glVertexAttribPointer(locationPos, 3, GL_FLOAT, false, 0, NULL);

		// Tell the GPU where the colors are: in the color buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboCol);
		glVertexAttribPointer(locationCol, 3, GL_FLOAT, false, 0, NULL);

		// Tell the GPU where the normals are: in the normal buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboNor);
		glVertexAttribPointer(locationNor, 3, GL_FLOAT, false, 0, NULL);

		// recursive draw
		unsigned int offset = 0;
		root_->drawScene(model, unifModel, unifModelInvTr, offset);

		// Shut off the information since we're done drawing.
		glDisableVertexAttribArray(locationPos);
		glDisableVertexAttribArray(locationCol);
		glDisableVertexAttribArray(locationNor);
	}
}