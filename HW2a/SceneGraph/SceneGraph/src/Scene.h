#pragma once

#include "Node.h"

class Scene {
public :
	Scene();
	~Scene();
	
	void setRoot(Node * root);
	Node * getRoot();

	Node * getNodeFromName(std::string& name);

	// Get the next node in pre-order traversal
	Node * getNextNode(Node * start);

	void uploadToBuffers(unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx);

	void drawScene(glm::mat4 model, unsigned int locationPos, unsigned int locationCol, unsigned int locationNor,
		unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx, unsigned int unifModel, unsigned int unifModelInvTr);
private:
	Node * root_;

	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> normals_;
	std::vector<glm::vec3> colors_;
	std::vector<GLuint> indices_;
};