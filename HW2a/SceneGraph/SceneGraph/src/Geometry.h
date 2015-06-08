#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

// An abstract base class for geometry in the scene graph.
class Geometry
{
public:
    // Enums for the types of geometry that your scene graph is required to contain.
    // Feel free to add more.
    enum geometryType {CUBE, SPHERE, CYLINDER, MESH};
    explicit Geometry(geometryType);
    virtual ~Geometry();

    // Function for building vertex data, i.e. vertices, colors, normals, indices.
    // Implemented in Sphere and Cylinder.
    virtual void buildGeomtery() = 0;

    // Getters
    const vector<glm::vec3>& getVertices() const
    {
        return vertices_;
    };
    const vector<glm::vec3>& getNormals() const
    {
        return normals_;
    };
    const vector<glm::vec3>& getColors() const
    {
        return colors_;
    };
    const vector<unsigned int>& getIndices() const
    {
        return indices_;
    };

    unsigned int getVertexCount() const
    {
        return vertices_.size();
    };
    unsigned int getIndexCount() const
    {
        return indices_.size();
    };

    const geometryType getGeometryType() const
    {
        return type_;
    };

	void setColor(glm::vec3 color)
	{
		for(unsigned int i = 0; i < colors_.size(); ++i)
			colors_[i] = color;
	}

protected:
    geometryType type_;

    vector<glm::vec3> vertices_;        // vertex buffer
    vector<glm::vec3> normals_;         // normal buffer
    vector<glm::vec3> colors_;          // color buffer
    vector<unsigned int> indices_;      // index buffer
};

#endif
