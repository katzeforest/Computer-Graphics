#pragma once

#include "Config.h"

class VoxelData
{
public:
	VoxelData(Config &config);
	~VoxelData();

	void generateSphere(Primitive &p);
	void generateCloud(Primitive &p);
	void generatePyroclastic(Primitive &p);

	inline unsigned int getVoxelID(unsigned int x, unsigned int y, unsigned int z) { return x*m_sizeY*m_sizeZ+y*m_sizeZ+m_sizeZ-1-z; }

	// public, use it as a structure since we have to modify it a lot
	float * m_densityBuffer;
	float * m_lightBuffer;
	unsigned int m_sizeX;
	unsigned int m_sizeY;
	unsigned int m_sizeZ;
};