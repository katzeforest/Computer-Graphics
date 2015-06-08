#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Sphere.h"
#include "Cylinder.h"
#include "Cube.h"
#include "Mesh.h"

Config::Config(void)
{
	s_instance = this;
}

Config * Config::s_instance = NULL;

#include <string>

Config::~Config(void)
{
	for (std::map<std::string, Material*>::iterator iter = m_materials.begin(); iter != m_materials.end(); ++iter)
	{
		delete iter->second;
	}
}

bool Config::init(char* filePath)
{
    std::ifstream fileStream (filePath, std::ifstream::in);
    
    // Check if file was opened successfully
    if (!fileStream.is_open())
        return false;

    std::string line;

	std::cout << "===== Started reading config file =====" << std::endl;

	// Parse header lines, assuming they could be in a any order
    while (std::getline(fileStream, line))
    {
		std::cout << line << std::endl;
		if (line == "")
			continue;

        if (line.length() >= 6 && !line.compare(0, 6, "CAMERA"))
		{
			unsigned int camParams = 5;
			for (unsigned int i = 0; i < camParams; ++i)
			{
				// Read next line
				std::getline(fileStream, line);
				std::istringstream valueStream(line.substr(4));

				if (line.length() < 4)
				{
					std::cerr << "Incorrect camera description" << std::endl;
					return false;
				}

				if (!line.compare(0, 4, "RESO"))
				{
					valueStream >> m_resolution[0] >> m_resolution[1];
					std::cout << "RESO " << m_resolution[0] << " " << m_resolution[1] << std::endl;
				}
				else if (!line.compare(0, 4, "EYEP"))
				{
					valueStream >> m_cameraPosition[0] >> m_cameraPosition[1] >> m_cameraPosition[2];
					std::cout << "EYEP " << m_cameraPosition[0] << " " << m_cameraPosition[1] << " " << m_cameraPosition[2] << std::endl;
				}
				else if (!line.compare(0, 4, "VDIR"))
				{
					valueStream >> m_cameraHeading[0] >> m_cameraHeading[1] >> m_cameraHeading[2];
					std::cout << "VDIR " << m_cameraHeading[0] << " " << m_cameraHeading[1] << " " << m_cameraHeading[2] << std::endl;
				}
				else if (!line.compare(0, 4, "UVEC"))
				{
					valueStream >> m_upVector[0] >> m_upVector[1] >> m_upVector[2];
					std::cout << "UVEC " << m_upVector[0] << " " << m_upVector[1] << " " << m_upVector[2] << std::endl;
				}
				else if (!line.compare(0, 4, "FOVY"))
				{
					valueStream >> m_fieldOfViewHalf;
					std::cout << "FOVY " << m_fieldOfViewHalf << std::endl;
				}
			}
		}

		if (line.length() >= 3 && !line.compare(0, 3, "MAT"))
		{
			std::string matName;
			matName = line.substr(4);

			glm::vec3 diffuseColor;
			glm::vec3 reflectiveColor;
			float specularExponent;
			float refractionIndex;
			bool mirror;
			bool trans;

			unsigned int matParams = 6;
			for (unsigned int i = 0; i < matParams; ++i)
			{
				// Read next line
				std::getline(fileStream, line);
				std::istringstream valueStream(line.substr(4));

				if (!line.compare(0, 4, "DIFF"))
				{
					valueStream >> diffuseColor[0] >> diffuseColor[1] >> diffuseColor[2];
					std::cout << "DIFF " << diffuseColor[0] << " " << diffuseColor[1] << " " << diffuseColor[2] << std::endl;
				}
				else if(!line.compare(0, 4, "REFL"))
				{
					valueStream >> reflectiveColor[0] >> reflectiveColor[1] >> reflectiveColor[2];
					std::cout << "REFL " << reflectiveColor[0] << " " << reflectiveColor[1] << " " << reflectiveColor[2] << std::endl;
				}	
				else if(!line.compare(0, 4, "EXPO"))
				{
					valueStream >> specularExponent;
					std::cout << "EXPO " << specularExponent << std::endl;
				}
				else if(!line.compare(0, 3, "IOR"))
				{
					valueStream >> refractionIndex;
					std::cout << "IOR " << refractionIndex << std::endl;
				}
				else if(!line.compare(0, 4, "MIRR")) // 1 = reflectance, 0 = opaque
				{
					valueStream >> mirror;
					std::cout << "MIRR " << mirror << std::endl;
				}
				else if(!line.compare(0, 4, "TRAN"))// 1 = transparent/refractive, 0 = opaque
				{
					valueStream >> trans;
					std::cout << "TRAN " << trans << std::endl;
				}
			}
			m_materials.insert( std::pair<std::string,Material*>(matName, new Material(diffuseColor, reflectiveColor, specularExponent, refractionIndex, mirror, trans)) );
		}

		if (line.length() >= 5 && !line.compare(0, 5, "LIGHT"))
		{
			unsigned int lightParams = 2;
			for (unsigned int i = 0; i < lightParams; ++i)
			{
				// Read next line
				std::getline(fileStream, line);
				std::istringstream valueStream(line.substr(4));

				if (line.length() < 10)
				{
					std::cerr << "Incorrect light description" << std::endl;
					return false;
				}

				if (!line.compare(0, 4, "LPOS"))
				{
					valueStream >> m_lightPosition[0] >> m_lightPosition[1] >> m_lightPosition[2];
					std::cout << "LPOS " << m_lightPosition[0] << " " << m_lightPosition[1] << " " << m_lightPosition[2] << std::endl;
					m_lightPosition[3] = 1.0f;
				}
				else if(!line.compare(0, 4, "LCOL"))
				{
					valueStream >> m_lightColor[0] >> m_lightColor[1] >> m_lightColor[2];
					std::cout << "LCOL " << m_lightColor[0] << " " << m_lightColor[1] << " " << m_lightColor[2] << std::endl;
				
				}	
			}
		}
		
		if(line.length() >= 4 && !line.compare(0, 4, "NODE"))
		{
			std::string nodeName;
			nodeName = line.substr(5);

			Geometry * nodeGeometry = NULL;
			glm::vec3 objectTranslation;
			glm::vec3 objectRotation;
			glm::vec3 objectScale;
			glm::vec3 transformationCenter;
			std::string objectParent;
			std::string objectShape;
			std::string matName;

			unsigned int nodeParams = 7;
			for (unsigned int i = 0; i < nodeParams; ++i)
			{
				std::getline(fileStream, line);

				if (!line.compare(0, 11, "TRANSLATION"))
				{
					std::istringstream valueStream(line.substr(11));
					valueStream >> objectTranslation[0] >> objectTranslation[1] >> objectTranslation[2];
					std::cout << "TRANSLATION " << objectTranslation[0] << " " << objectTranslation[1] << " " << objectTranslation[2] << std::endl;
				}
				else if (!line.compare(0, 8, "ROTATION"))
				{
					std::istringstream valueStream(line.substr(8));
					valueStream >> objectRotation[0] >> objectRotation[1] >> objectRotation[2];

					std::cout << "ROTATION " << objectRotation[0] << " " << objectRotation[1] << " " << objectRotation[2] << std::endl;
				
					// Convert to radians
					static const float PI = 3.141592653589f;
					objectRotation[0] *= PI/180;
					objectRotation[1] *= PI/180;
					objectRotation[2] *= PI/180;

				}
				else if (!line.compare(0, 5, "SCALE"))
				{
					std::istringstream valueStream(line.substr(5));
					valueStream >> objectScale[0] >> objectScale[1] >> objectScale[2];
					std::cout << "SCALE " << objectScale[0] << " " << objectScale[1] << " " << objectScale[2] << std::endl;
				}
				else if (!line.compare(0, 6, "CENTER"))
				{
					std::istringstream valueStream(line.substr(6));
					valueStream >> transformationCenter[0] >> transformationCenter[1] >> transformationCenter[2];
					std::cout << "CENTER " << transformationCenter[0] << " " << transformationCenter[1] << " " << transformationCenter[2] << std::endl;
				}
				else if (!line.compare(0, 6, "PARENT"))
				{
					std::istringstream valueStream(line.substr(6));
					valueStream >> objectParent;
					std::cout << "PARENT " << objectParent << std::endl;
				}
				else if (!line.compare(0, 5, "SHAPE"))
				{
					std::istringstream valueStream(line.substr(5));
					valueStream >> objectShape;
					if (objectShape == "sphere")
						nodeGeometry = new Sphere();		
					else if (objectShape == "cylinder")
						nodeGeometry = new Cylinder();
					else if (objectShape == "cube")
						nodeGeometry = new Cube();
					else if (objectShape == "mesh")
					{
						std::getline(fileStream, line);
						std::istringstream valueStream(line.substr(4));

						string path;
						valueStream >> path;

						path = "obj/" + path;

						
						nodeGeometry = new Mesh(path);
					}
					else if (objectShape == "null")
						nodeGeometry = new EmptyGeometry();
					else
					{
						std::cerr << "Unknown SHAPE : " << objectShape << std::endl;
						return false;
					}

					std::cout << "SHAPE " << objectShape << std::endl;
				}
				else if (!line.compare(0, 3, "MAT"))
				{
					matName = line.substr(4);
				}
			}

			Node *parentPointer;

			if (objectParent == "null")
				parentPointer = NULL;
			else
				parentPointer = m_scene.getNodeFromName(objectParent);

			Node * newNode = new Node(parentPointer, nodeGeometry, nodeName, objectTranslation, objectRotation, objectScale, transformationCenter, findMaterial(matName));

			if (parentPointer)
				parentPointer->addChild(newNode);
			else
				m_scene.setRoot(newNode);
		}
	}

	std::cout << "===== Finished reading config file =====" << std::endl;
	// All went fine
	return true;
}



