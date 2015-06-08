#include "VoxelData.h"
#include "Config.h"

VoxelData::VoxelData(Config &myConfig)
{
	unsigned int * voxelBufSizeArray = myConfig.getVoxelBufferSize();
	m_sizeX = voxelBufSizeArray[0];
	m_sizeY = voxelBufSizeArray[1];
	m_sizeZ = voxelBufSizeArray[2];
	unsigned int voxelBufSize = m_sizeX * m_sizeY * m_sizeZ;

	m_lightBuffer = new float[voxelBufSize];
	for (unsigned int i=0; i < voxelBufSize; ++i)
		m_lightBuffer[i] = -1;

	m_densityBuffer = myConfig.getVoxelBuffer();
}

VoxelData::~VoxelData()
{
	delete[] m_lightBuffer;
}