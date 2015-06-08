How To Run
  Put all the test.txt in the debug default folder. Open HW1a.sln in VS 2010.  Right click the Solution and go to Properties -> Configuration Properties -> Debugging -> Command Arguments to change different test files. Choose release to run the project.

Files Includes With This Project:
main.cpp  Vector.cpp  Config.cpp  Camera.cpp  VoxelData.cpp
          Vector.h    Config.h    Camera.h    VoxelData.h
          main.cpp    EasyBMP.cpp EasyBMP headers

Homework # 1b
Requirements:
1. Voxel Buffer
  The voxel data class stores voxel data and methods. 
class VoxelData
{
public:
	VoxelData(Config &config);
	~VoxelData();

	inline unsigned int getVoxelID(unsigned int x, unsigned int y, unsigned int z) 
{ return x*m_sizeY*m_sizeZ+y*m_sizeZ+m_sizeZ-1-z; }

	float * m_densityBuffer;
	float * m_lightBuffer;
	unsigned int m_sizeX;
	unsigned int m_sizeY;
	unsigned int m_sizeZ;
};

2. Ray March
  The Ray March Method is in void Camera::renderRayMarching()
. It is the same algorithm as in the slides#312. It will output a debug image named ¡°FILE_raymarching.bmp¡± which FILE is the filename given to the output image. Please check the code for details.


Extra Credit:
  N/A