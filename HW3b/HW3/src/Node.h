#pragma once

#include <vector>
#include "Geometry.h"
#include "Material.h"
#include <GL/glut.h>

class Node {
public:
	Node(Node* parent, Geometry* geometry, std::string name, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale, glm::vec3& center, Material* material);
	~Node();

	void addChild(Node* child);
	Node * findChildByName(std::string& name);

	Node * getParent() { return parent_; }
	
	unsigned int getChildrenCount() { return children_.size(); };
	Node * getChild(unsigned int index) { return (index < children_.size()) ? children_[index] : NULL; }

	void deleteAllChildren();
	void deleteChild(Node * child);

	glm::vec3& getTranslationVector() { return translation_; }
	glm::vec3& getRotationVector() { return rotation_; }
	glm::vec3& getScaleVector() { return scale_; }
	glm::vec3& getTransformationCenterPoint() { return center_; }

	std::string& getName() {return name_;}

	void setColor(glm::vec3 color); // Apply a new temporary color to geometry
	void resetColor(); // Restore original color to geometry

	Material* getMaterial() { return material_; }

	void buildArrays(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<glm::vec3> &colors, std::vector<GLuint> &indices);
	void drawScene(glm::mat4 model, unsigned int unifModel, unsigned int unifModelInvTr, unsigned int &bufferOffset);

	Intersection Node::intersect(glm::mat4 T, Ray ray_world);

private:
	std::string name_;
	std::vector<Node*> children_;
	Node *parent_;
	Geometry* geometry_;
	glm::vec3 translation_;
	glm::vec3 rotation_;
	glm::vec3 scale_;
	glm::vec3 center_;
	Material* material_;
};