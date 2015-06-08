#pragma once
#include "Config.h"


class Camera
{
private:    
	glm::vec3	    m_position;
	glm::vec3		m_direction;
	glm::vec3		m_up;
	int				m_fovY; // half fov angle
	double          m_fovRad; // half fov angle in radians

public:
    Camera(glm::vec3& position, glm::vec3& direction, glm::vec3& upVector, int fov);
    ~Camera(void);

	void debugRender(unsigned int width, unsigned int height, std::string& outputFile);
	void renderRayBoxIntersection(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile);
	//void Camera::MoveBox(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], Vector m_newPosition, std::string& outputFile);
	//void Camera::renderRayMarching(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile, VoxelData &voxelData, float step, Vector& lightPosition, float lightColor[]);
};

