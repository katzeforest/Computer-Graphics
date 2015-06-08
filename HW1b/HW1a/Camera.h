#pragma once
#include "Vector.h" 
#include "Config.h"
#include "VoxelData.h"
#include <EasyBMP.h>

class Camera
{
private:    
	Vector			m_position;
	Vector			m_direction;
	Vector			m_up;
	int				m_fovY; // half fov angle
	double          m_fovRad; // half fov angle in radians

public:
    Camera(Vector& position, Vector& direction, Vector& upVector, int fov);
    ~Camera(void);

	void debugRender(unsigned int width, unsigned int height, std::string& outputFile);
	void renderRayBoxIntersection(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile);
	void Camera::MoveBox(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], Vector m_newPosition, std::string& outputFile);
	void Camera::renderRayMarching(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile, VoxelData &voxelData, float step, Vector& lightPosition, float lightColor[]);
};

