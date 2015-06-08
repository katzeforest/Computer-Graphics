#ifndef CONFIG_H 
#define CONFIG_H
#include <string>
#include "Scene.h"
#include "Geometry.h"


enum PrimitiveType{PrimitiveType_INVALID, PrimitiveType_Sphere, PrimitiveType_Cube, PrimitiveType_Cylinder};
/*
class Primitive
{
private:
	PrimitiveType	m_type;
	Vector			m_center;
	float			m_radius;

public:
	Primitive() {}
	Primitive(PrimitiveType	type, double posX, double posY, double posZ,float radius) :
		m_type(type), m_center(posX, posY, posZ), m_radius(radius) {}

	PrimitiveType getPrimitiveType() { return m_type; }
	Vector & getCenterPosition() { return m_center; }
	float getRadius() { return m_radius; }

	void init(PrimitiveType	type, double posX, double posY, double posZ,float radius)
	{
		m_type = type;
		m_center[0] = posX; m_center[1] = posY; m_center[2] = posZ;
		m_radius = radius;
	}
};
*/

class Config
{
private:

    //float        m_backgroundColor[3];	// Background color RGB

    unsigned int m_resolution[2];		// Width, Height in pixels
    glm::vec3    m_cameraPosition;		// Position of the camera
    glm::vec3    m_cameraHeading;		// Direction the camera is looking at
    glm::vec3	 m_upVector;			// Direction of the up vector
    int          m_fieldOfViewHalf;		// The half angle field of view in Y direction in degrees
    glm::vec3	 m_lightPosition;		// The position of the light
    float        m_lightColor[3];		// The color of the light
	Scene        m_scene;				// Link to the scene

public:
    Config(void);
    ~Config(void);

	// Initialize values from a file
    bool init(char* filePath);
	
	// Getters
    unsigned int*	getResolution()			{ return m_resolution; }		//RESO
    glm::vec3&		getCameraPosition()		{ return m_cameraPosition; }	//EYEP
    glm::vec3&		getCameraHeading()		{ return m_cameraHeading; }		//VDIR
    glm::vec3&		getUpVector()			{ return m_upVector; }			//UVEC
    int				getFieldOfViewHalf()	{ return m_fieldOfViewHalf; }	//FOVY
    glm::vec3&	    getLightPosition()		{ return m_lightPosition; }		//LPOS
    float*			getLightColor()			{ return m_lightColor; }		//LCOL
	Scene&			getScene()				{ return m_scene; }
};

#endif

