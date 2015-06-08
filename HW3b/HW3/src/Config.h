#ifndef CONFIG_H 
#define CONFIG_H
#include <string>
#include <map>

#include "Geometry.h"
#include "Scene.h"
#include "Material.h"

enum PrimitiveType{PrimitiveType_INVALID, PrimitiveType_Sphere, PrimitiveType_Cube, PrimitiveType_Cylinder};


class Config
{
private:

    //float        m_backgroundColor[3];	// Background color RGB

    unsigned int m_resolution[2];		// Width, Height in pixels
    glm::vec3    m_cameraPosition;		// Position of the camera
    glm::vec3    m_cameraHeading;		// Direction the camera is looking at
    glm::vec3	 m_upVector;			// Direction of the up vector
    int          m_fieldOfViewHalf;		// The half angle field of view in Y direction in degrees
    glm::vec4	 m_lightPosition;		// The position of the light
    float        m_lightColor[3];		// The color of the light
	Scene		m_scene;				// Link to the scene

	std::map<std::string, Material*> m_materials; // List of materials

	static Config * s_instance;

public:
    Config(void);
    ~Config(void);

	static Config * getInstance() {if (s_instance) return s_instance; else return s_instance = new Config();}

	// Initialize values from a file
    bool init(char* filePath);
	
	// Getters
    unsigned int*	getResolution()			{ return m_resolution; }		//RESO
    glm::vec3&		getCameraPosition()		{ return m_cameraPosition; }	//EYEP
    glm::vec3&		getCameraHeading()		{ return m_cameraHeading; }		//VDIR
    glm::vec3&		getUpVector()			{ return m_upVector; }			//UVEC
    int				getFieldOfViewHalf()	{ return m_fieldOfViewHalf; }	//FOVY
    glm::vec4&	    getLightPosition()		{ return m_lightPosition; }		//LPOS
    float*			getLightColor()			{ return m_lightColor; }		//LCOL
	Scene&			getScene()				{ return m_scene; }
	Material*       findMaterial(std::string& name)  { return m_materials[name]; }
};

#endif

