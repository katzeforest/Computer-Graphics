#include "VoxelData.h"
#include "Config.h"
#include "perlin.h"
#include "math.h"

VoxelData::VoxelData(Config &myConfig)
{
	unsigned int * voxelBufSizeArray = myConfig.getVoxelBufferSize();
	m_sizeX = voxelBufSizeArray[0];
	m_sizeY = voxelBufSizeArray[1];
	m_sizeZ = voxelBufSizeArray[2];
	m_mostDense = 1.0f;

	unsigned int voxelBufSize = m_sizeX * m_sizeY * m_sizeZ;

	m_lightBuffer = new float[voxelBufSize];
	m_densityBuffer = new float[voxelBufSize];
	for (unsigned int i=0; i < voxelBufSize; ++i)
	{
		m_lightBuffer[i] = -1;
		m_densityBuffer[i] = 0;
	}

	for (unsigned int i = 0; i < myConfig.getPrimitiveCount(); ++i)
	{
		Primitive &p = myConfig.getPrimitiveArray()[i];
		switch (p.getPrimitiveType())
		{
		
		case PrimitiveType_Sphere:
			generateSphere(p);
			break;
		case PrimitiveType_Cloud:
			generateCloud(p);
			break;
		case PrimitiveType_Pyroclastic:
			generatePyroclastic(p);
			break;
		default:
		case PrimitiveType_INVALID:
			break;
		}
	}
}

VoxelData::~VoxelData()
{
	delete[] m_lightBuffer;
	delete[] m_densityBuffer;
}

void VoxelData::generateSphere(Primitive &p)
{
	// input
	Vector mySphereCenter = p.getCenterPosition();
	float mySphereRadius = p.getRadius();

	// for computing voxel center
	Vector voxelSize(1.0f/m_sizeX, 1.0f/m_sizeY, 1.0f/m_sizeZ);
	Vector halfVoxelSize(0.5/m_sizeX, 0.5/m_sizeY, 0.5/m_sizeZ);
	Vector MinPoint(0, 0, -1);

	Vector myVoxelCenter ;

	float dis;   // distance from Sphere center to voxel center

	for (int i =0; i < m_sizeX; i++)
		for(int j =0; j < m_sizeY; j++)
			for(int k =0; k < m_sizeZ; k++)
				{
					myVoxelCenter = voxelSize * Vector(i, j, k) + halfVoxelSize + MinPoint;  // Get voxel center
					dis = (mySphereCenter - myVoxelCenter ).length();
					if(dis <= mySphereRadius)
					{
						unsigned int index = getVoxelID(i,j,k);
						m_densityBuffer[index] += 1.0f*(1.0-dis/mySphereRadius)*100;   //Density is a linear value
						if (m_mostDense < m_densityBuffer[index])
							m_mostDense = m_densityBuffer[index];
					}
				}
}

void VoxelData::generateCloud(Primitive &p)
{	
    Vector myCloudCenter = p.getCenterPosition();
	float myCloudRadius = p.getRadius();

	// for computing voxel center
	Vector voxelSize(1.0f/m_sizeX, 1.0f/m_sizeY, 1.0f/m_sizeZ);
	Vector halfVoxelSize(0.5/m_sizeX, 0.5/m_sizeY, 0.5/m_sizeZ);
	Vector MinPoint(0, 0, -1);

	Perlin perlin(6, 0.125, 2 ,1);

	float dis;
	float scale = 100; //1.0/(m_sizeX * m_sizeY * m_sizeZ);

	for (int i =0; i < m_sizeX; i++)
		for(int j =0; j < m_sizeY; j++)
			for(int k =0; k < m_sizeZ; k++)
				{
					Vector myVoxelCenter(voxelSize * Vector(i, j, k) + halfVoxelSize + MinPoint);  // Get voxel center
					dis = ( myVoxelCenter - myCloudCenter ).length(); // |P|

					// noiseDensity(P) = (fbm(P*scale)+(1 - |P|/radius))*scale
					float noiseDensitytemp = (perlin.Get((myVoxelCenter-myCloudCenter) * scale) + (1 - dis/myCloudRadius)) * scale; 

					if(noiseDensitytemp < 0)
						noiseDensitytemp = 0 ;

					unsigned int index = getVoxelID(i,j,k);

					m_densityBuffer[index] += noiseDensitytemp;
					
					if (m_mostDense < m_densityBuffer[index])
						m_mostDense = m_densityBuffer[index];
				}
				
}

void VoxelData::generatePyroclastic(Primitive &p)
{
    Vector myPyroclasticCenter = p.getCenterPosition();
	float myPyroclasticRadius = p.getRadius();

	// for computing voxel center
	Vector voxelSize(1.0f/m_sizeX, 1.0f/m_sizeY, 1.0f/m_sizeZ);
	Vector halfVoxelSize(0.5/m_sizeX, 0.5/m_sizeY, 0.5/m_sizeZ);
	Vector MinPoint(0, 0, -1);

	Perlin perlin(6, 0.125, 2, 1);

	float dis;
	float scale = 100; //1.0/(m_sizeX * m_sizeY * m_sizeZ);

	for (int i =0; i < m_sizeX; i++)
		for(int j =0; j < m_sizeY; j++)
			for(int k =0; k < m_sizeZ; k++)
				{
					Vector myVoxelCenter(voxelSize * Vector(i, j, k) + halfVoxelSize + MinPoint);  // Get voxel center
					dis = ( myVoxelCenter - myPyroclasticCenter ).length(); // |P|

					// pyroclasticDensity(P) 
					float pyroclasticDensitytemp = myPyroclasticRadius * scale - dis/myPyroclasticRadius + abs(perlin.Get((myVoxelCenter-myPyroclasticCenter)*scale));

					if(pyroclasticDensitytemp < 0)
						pyroclasticDensitytemp = 0 ;

					unsigned int index = getVoxelID(i,j,k);
					m_densityBuffer[index] += pyroclasticDensitytemp*scale;

					if (m_mostDense < m_densityBuffer[index])
						m_mostDense = m_densityBuffer[index];
				}
				
}