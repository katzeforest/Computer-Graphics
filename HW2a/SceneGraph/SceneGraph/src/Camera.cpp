#include "Camera.h"
#include "Config.h"
//#include "EasyBMP.h"
#include <cmath>
#include <algorithm>
#include <limits>

#define M_PI 3.14159265358979323846

Camera::Camera(glm::vec3& position,  glm::vec3& direction,  glm::vec3& upVector, int fov)
	: m_position(position), m_direction(direction), m_up(upVector)
{
	m_fovY = fov; // half angle
	m_fovRad = fov * M_PI/180;
} 

Camera::~Camera(void)
{
}

bool RayBoxIntersection(double t1x,double t2x, double t1y, double t2y, double t1z,double t2z, double & Tnear, double & Tfar ) {
	
	Tnear = -std::numeric_limits<float>::infinity();
	Tfar = std::numeric_limits<float>::infinity();

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

// This is just for compatibility with the previous camera code
bool RayBoxIntersection(double t1x,double t2x, double t1y, double t2y, double t1z,double t2z)
{
	double Tnear, Tfar; 
	// get the value of Tnear and Tfar
	return RayBoxIntersection(t1x, t2x, t1y, t2y, t1z, t2z, Tnear, Tfar);
}

