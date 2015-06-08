#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>

Config::Config(void)
{
    m_primitives = NULL;
}


#include <string>

Config::~Config(void)
{
	if (m_primitives)
		delete[] m_primitives;
}

bool Config::init(char* filePath)
{
    std::ifstream fileStream (filePath, std::ifstream::in);
    
    // Check if file was opened successfully
    if (!fileStream.is_open())
        return false;

    std::string line;

	// Parse header lines, assuming they could be in a any order
    while (std::getline(fileStream, line))
    {
		std::cout << "Parsing line: " << line << std::endl;
		if (line == "")
			continue;
	    if( line.length() < 4 )
			   break;

		std::istringstream valueStream(line.substr(4));

        if (!line.compare(0, 4, "STEP"))
		{
			valueStream >> m_step;
		}
		else if (!line.compare(0, 4, "XYZC"))
		{
			valueStream >> m_voxelBufferSize[0] >> m_voxelBufferSize[1] >> m_voxelBufferSize[2] ;
		}
		else if (!line.compare(0, 4, "BRGB"))
		{
			valueStream >> m_backgroundColor[0] >> m_backgroundColor[1] >> m_backgroundColor[2];
		}
		else if (!line.compare(0, 4, "MRGB"))
		{
			valueStream >> m_materialColor[0] >> m_materialColor[1] >> m_materialColor[2];
		}
		else if (!line.compare(0, 4, "FILE"))
		{
			m_outputFileName = valueStream.str();
			m_outputFileName.erase(0, m_outputFileName.find_first_not_of(' ')); //trim leading spaces
		}
		else if (!line.compare(0, 4, "RESO"))
		{
			valueStream >> m_resolution[0] >> m_resolution[1];
		}
		else if (!line.compare(0, 4, "EYEP"))
		{
			valueStream >> m_cameraPosition[0] >> m_cameraPosition[1] >> m_cameraPosition[2];
		}
		else if (!line.compare(0, 4, "VDIR"))
		{
			valueStream >> m_cameraHeading[0] >> m_cameraHeading[1] >> m_cameraHeading[2];
		}
		else if (!line.compare(0, 4, "UVEC"))
		{
			valueStream >> m_upVector[0] >> m_upVector[1] >> m_upVector[2];
		}
		else if (!line.compare(0, 4, "FOVY"))
		{
			valueStream >> m_fieldOfViewHalf;
		}
		else if (!line.compare(0, 4, "LPOS"))
		{
			valueStream >> m_lightPosition[0] >> m_lightPosition[1] >> m_lightPosition[2];
		}
		else if (!line.compare(0, 4, "LCOL"))
		{
			valueStream >> m_lightColor[0] >> m_lightColor[1] >> m_lightColor[2];
		}
		else if (!line.compare(0, 4, "NPOS"))
		{
			valueStream >> m_newPosition[0] >> m_newPosition[1] >> m_newPosition[2];
		}
		else
			break; // Unknown header, this must be the start of primitives
    }



    // Read primitives

    // At this point the number of primitives in in line var
    std::istringstream(line) >> m_primitiveCount;

	// Allocate the array for the primitives
	m_primitives = new Primitive[m_primitiveCount];

	for (unsigned int x = 0; x < m_primitiveCount; ++x)
	{
		// Skip the blank line
		do
		{
			std::getline(fileStream, line);
		} while (line == "");

		// Read primitive type
		PrimitiveType type;
		
		if (line == "sphere")
			type = PrimitiveType_Sphere;
		else if (line == "pyroclastic")
			type = PrimitiveType_Pyroclastic;
		else if (line == "cloud")
			type = PrimitiveType_Cloud;
		else
		{
			type = PrimitiveType_INVALID;
			std::cerr << "Uknown primitive type: " << line << std::endl;
		}

		double posX, posY, posZ, radius;

		// Read center position
        fileStream >> posX >> posY >> posZ;

		// Read radius
		fileStream >> radius;

		m_primitives[x].init(type, posX, posY, posZ, radius);
	}

	return true;
}

std::ostream & operator<<(std::ostream &os, const Config& c)
{
	os << "STEP " << c.m_step << std::endl;
	os << "XYZC " << c.m_voxelBufferSize[0] << " " << c.m_voxelBufferSize[1] << " " << c.m_voxelBufferSize[2] << std::endl;
	os << "BRGB " << c.m_backgroundColor[0] << " " << c.m_backgroundColor[1] << " " << c.m_backgroundColor[2] << std::endl;
	os << "MRGB " << c.m_materialColor[0] << " " << c.m_materialColor[1] << " " << c.m_materialColor[2] << std::endl;
	os << "FILE " << c.m_outputFileName << std::endl;
	os << "RESO " << c.m_resolution[0] << " " << c.m_resolution[1] << std::endl;
	os << "EYEP " << c.m_cameraPosition[0] << " " << c.m_cameraPosition[1] << " " << c.m_cameraPosition[2] << std::endl;
	os << "VDIR " << c.m_cameraHeading[0] << " " << c.m_cameraHeading[1] << " " << c.m_cameraHeading[2] << std::endl;
	os << "UVEC " << c.m_upVector[0] << " " << c.m_upVector[1] << " " << c.m_upVector[2] << std::endl;
	os << "FOVY " << c.m_fieldOfViewHalf << std::endl;
	os << "LPOS " << c.m_lightPosition[0] << " " << c.m_lightPosition[1] << " " << c.m_lightPosition[2] << std::endl;
	os << "LCOL " << c.m_lightColor[0] << " " << c.m_lightColor[1] << " " << c.m_lightColor[2] << std::endl;
	os << "NPOS " << c.m_newPosition[0] << " " << c.m_newPosition[1] << " " << c.m_newPosition[2] << std::endl;

	return os;
}