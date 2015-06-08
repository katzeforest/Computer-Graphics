#ifndef CONFIG_H 
#define CONFIG_H
#include <string>
#include "Vector.h"

class Config
{
private:
    float        m_step;				// The step size to use in your rendering, a float
    unsigned int m_voxelBufferSize[3];	// Voxel buffer size x,y,z
    float        m_backgroundColor[3];	// Background color RGB
    float        m_materialColor[3];	// Material color RGB
    std::string  m_outputFileName;
    unsigned int m_resolution[2];		// Width, Height in pixels
    Vector		 m_cameraPosition;		// Position of the camera
    Vector		 m_cameraHeading;		// Direction the camera is looking at
    Vector		 m_upVector;			// Direction of the up vector
    int          m_fieldOfViewHalf;		// The half angle field of view in Y direction in degrees
    Vector		 m_lightPosition;		// The position of the light
    float        m_lightColor[3];		// The color of the light

	Vector       m_newPosition;          //Move the min point by vector

    float*       m_voxelBuffer;

public:
    Config(void);
    ~Config(void);

	// Initialize values from a file
    bool init(char* filePath);
	
	// Getters
    float			getStep() const			{ return m_step; }				//STEP
    unsigned int*	getVoxelBufferSize()	{ return m_voxelBufferSize; }	//XYZC
    float*			getBackgroundColor()	{ return m_backgroundColor; }	//BRGB
    float*			getMaterialColor()		{ return m_materialColor; }		//MRGB
    std::string&	getOutputFileName()		{ return m_outputFileName; }	//FILE
    unsigned int*	getResolution()			{ return m_resolution; }		//RESO
    Vector&			getCameraPosition()		{ return m_cameraPosition; }	//EYEP
    Vector&			getCameraHeading()		{ return m_cameraHeading; }		//VDIR
    Vector&			getUpVector()			{ return m_upVector; }			//UVEC
    int				getFieldOfViewHalf()	{ return m_fieldOfViewHalf; }	//FOVY
    Vector&			getLightPosition()		{ return m_lightPosition; }		//LPOS
    float*			getLightColor()			{ return m_lightColor; }		//LCOL

	Vector&			getNewPosition()		{ return m_newPosition; }		//NPOS

	float*			getVoxelBuffer()		{ return m_voxelBuffer; }		//The rest of the values

	friend std::ostream & operator<<(std::ostream &os, const Config& c);
};

#endif

