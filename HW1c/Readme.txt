How To Run
  Put all the test.txt in the debug default folder. Open HW1a.sln in VS 2010.  Right click the Solution and go to Properties -> Configuration Properties -> Debugging -> Command Arguments to change different test files. Choose release to run the project.

Files Includes With This Project:
main.cpp  Vector.cpp  Config.cpp  Camera.cpp  VoxelData.cpp
          Vector.h    Config.h    Camera.h    VoxelData.h
          main.cpp    EasyBMP.cpp EasyBMP headers
          perlin.h     perlin.cpp

Homework # 1c
Requirements:
1. Rasterization Primitives
    It is done in the three methods.
	void init(PrimitiveType	type, double posX, double posY, double posZ,float radius)
	{
		m_type = type;
		m_center[0] = posX; m_center[1] = posY; m_center[2] = posZ;
		m_radius = radius;
	}

2. Spheres
    It is in VoxelData::generateSphere(Primitive &p) in VoxelDate.cpp.
3. Noise
    It is in VoxelData::generateCloud(Primitive &p) in VoxelData.cpp.
4. Pyroclastics
    It is in VoxelData::generatePyroclastic(Primitive &p)) in VoxelData.cpp.
5. Configuration File
    Please check the config.cpp and config.h.


Extra Credit:
  N/A