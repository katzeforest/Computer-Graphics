#include "Camera.h"
#include "Config.h"
#include "Vector.h"
#include "EasyBMP.h"
#include <cmath>
#include <algorithm>
#include <limits>

#define M_PI 3.14159265358979323846

Camera::Camera(Vector& position,  Vector& direction,  Vector& upVector, int fov)
	: m_position(position), m_direction(direction), m_up(upVector)
{
	m_fovY = fov; // half angle
	m_fovRad = fov * M_PI/180;
} 

Camera::~Camera(void)
{
}

bool RayBoxIntersection(double t1x,double t2x, double t1y, double t2y, double t1z,double t2z, double & Tnear, double & Tfar ) {
	
	Tnear = -std::numeric_limits<float>::infinity();
	Tfar = std::numeric_limits<float>::infinity();

	//X direction
	if(t1x > t2x) {std::swap(t1x, t2x);}
	if(Tnear < t1x) {Tnear = t1x;}
	if(Tfar > t2x)  {Tfar = t2x;}

	//Y direction
	if(t1y > t2y) {std::swap(t1y, t2y);}
	if(Tnear < t1y) {Tnear = t1y;}
	if(Tfar > t2y)  {Tfar = t2y;}

	//Z direction
	if(t1z > t2z) {std::swap(t1z, t2z);}
	if(Tnear < t1z) {Tnear = t1z;}
	if(Tfar > t2z)  {Tfar = t2z;}


	return (Tnear < Tfar);	
}

// This is just for compatibility with the previous camera code
bool RayBoxIntersection(double t1x,double t2x, double t1y, double t2y, double t1z,double t2z)
{
	double Tnear, Tfar; 
	// get the value of Tnear and Tfar
	return RayBoxIntersection(t1x, t2x, t1y, t2y, t1z, t2z, Tnear, Tfar);
}

void Camera::renderRayMarching(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile, VoxelData &voxelData, float stepsize, Vector& lightPosition, float lightColor[])
{

	BMP file;
	file.SetSize(width,height);
	file.SetBitDepth(24);

	Vector myVectorEyetoMiddle(m_direction.normalize());

	double tanfovx = tan(m_fovRad)*double(width)/height ;  
    Vector myEyeVectorX = myVectorEyetoMiddle.crossProduct(m_up) ;     //A
	Vector myEyeVectorY = myEyeVectorX.crossProduct(myVectorEyetoMiddle) ;     //B

	Vector myMiddlePoint = myVectorEyetoMiddle + m_position;

	Vector myImageVectorX = myEyeVectorX *  ( myVectorEyetoMiddle.normalize().length() * tanfovx / myEyeVectorX.length() ); //H
	Vector myImageVectorY = myEyeVectorY *  ( myVectorEyetoMiddle.normalize().length() * tan(m_fovRad) / myEyeVectorY.length() ); //V
		
	float UnitCube[2][3] = { {0, 0, -1},  {1, 1, 0} };  //unit cube boundaries

	Vector MinPoint (UnitCube[0][0], UnitCube[0][1], UnitCube[0][2]);
	Vector MaxPoint (UnitCube[1][0], UnitCube[1][1], UnitCube[1][2]);

	double a1 = UnitCube[0][0] - m_position[0];
	double a2 = UnitCube[1][0] - m_position[0];
	double b1 = UnitCube[0][1] - m_position[1];
	double b2 = UnitCube[1][1] - m_position[1];
	double c1 = UnitCube[0][2] - m_position[2];
	double c2 = UnitCube[1][2] - m_position[2];

    double T;   //Transmittance
	float k = 1.0f;    //can set k=1 to 2

	RGBApixel BGColor; 
	BGColor.Red = (ebmpBYTE) (m_bgcolor[0]*255);
	BGColor.Green = (ebmpBYTE) (m_bgcolor[1]*255);
	BGColor.Blue = (ebmpBYTE) (m_bgcolor[2]*255);
	BGColor.Alpha = 0;

	Vector voxelSize(1.0f/voxelData.m_sizeX, 1.0f/voxelData.m_sizeY, 1.0f/voxelData.m_sizeZ);
	Vector halfVoxelSize(0.5/voxelData.m_sizeX, 0.5/voxelData.m_sizeY, 0.5/voxelData.m_sizeZ);

	for(unsigned int j=0;j<height;j++)
	{
		for(unsigned int i=0;i<width;i++)
		{
			//Ray Marching
			// Initialize
			float C[3] = {0, 0, 0};                    //color
			T = 1;

			double sx,sy;
			sx = double(i)/(width-1);
			sy = double(j)/(height-1);
			
			Vector P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;
			Vector myRayCasting = (P - m_position)*(1.0 / (P - m_position).normalize().length()); //RayDirection = (P - E)/normalize(P - E);
		 
			double t1x =  a1 / myRayCasting[0];
			double t2x =  a2 / myRayCasting[0];
			double t1y =  b1 / myRayCasting[1];
			double t2y =  b2 / myRayCasting[1];
			double t1z =  c1 / myRayCasting[2];
			double t2z =  c2 / myRayCasting[2];

			double tnear, tfar;
			bool hit = RayBoxIntersection( t1x, t2x, t1y, t2y, t1z, t2z, tnear, tfar );
			
			if( !hit )
			{
				file.SetPixel(i, height-j-1, BGColor);
			}
			else 
			{ // Hit: compute the pixel color

				Vector Pnear = m_position + myRayCasting*tnear;
				Vector Pfar = m_position + myRayCasting*tfar;
				float maxD = (Pnear-Pfar).length();		// Distance between the entrance and exit points

				Vector stepVector = myRayCasting.normalize() * stepsize;
				Vector xi(Pnear); // Start at the entrance point

				for( float distance = 0.0f ; distance < maxD; distance += stepsize)
				{
					// Convert coordinates to voxel number
					unsigned int indexX = std::min(unsigned int ((xi[0] - UnitCube[0][0])/voxelSize[0]), voxelData.m_sizeX-1);
					unsigned int indexY = std::min(unsigned int ((xi[1] - UnitCube[0][1])/voxelSize[1]), voxelData.m_sizeY-1); 
					unsigned int indexZ = std::min(unsigned int ((xi[2] - UnitCube[0][2])/voxelSize[2]), voxelData.m_sizeZ-1);

					unsigned int voxelID = voxelData.getVoxelID(indexX, indexY, indexZ);

					float voxelLight = voxelData.m_lightBuffer[voxelID];
					float voxelDensity = voxelData.m_densityBuffer[voxelID];

					if (voxelLight  < 0) // Light not processed yet
					{
						// Compute light
						Vector voxelCenterPoint = voxelSize * Vector(indexX, indexY, indexZ) + halfVoxelSize + MinPoint;
						Vector lightDir = (voxelCenterPoint - lightPosition);   //Direction from light to voxel

						float distolight = lightDir.length() - 4*stepsize; //+ stepsize; //Add stepsize to make sure not to miss the target voxel 
 						float Tlight = 1 ;

						Vector stepLightVector = lightDir.normalize() * stepsize; // Use the same stepsize for light 

						unsigned int lastVisitedVoxel = std::numeric_limits<int>::max(); // Keep trace of the last visited voxel to avoid calculating the light twice

						Vector xl(lightPosition);

						for(float d = 0.0f; d < distolight; d += stepsize)
						{
							if (xl[0] < MinPoint[0] || xl[1] < MinPoint[1] || xl[2] < MinPoint[2]
								|| xl[0] > MaxPoint[0] || xl[1] > MaxPoint[1] || xl[2] > MaxPoint[2])
							{
								// outside the box
								xl += stepLightVector;
								continue;
							}

							// Convert coordinates to voxel number
							unsigned int indexX = std::min(unsigned int ((xl[0] - UnitCube[0][0])/voxelSize[0]), voxelData.m_sizeX-1);
							unsigned int indexY = std::min(unsigned int ((xl[1] - UnitCube[0][1])/voxelSize[1]), voxelData.m_sizeY-1); 
							unsigned int indexZ = std::min(unsigned int ((xl[2] - UnitCube[0][2])/voxelSize[2]), voxelData.m_sizeZ-1);

							unsigned int index = voxelData.getVoxelID(indexX, indexY, indexZ);

							if (lastVisitedVoxel != index && index != voxelID)
							{
								Tlight *= exp(-voxelData.m_densityBuffer[index]);
								lastVisitedVoxel = index;
							}
							// Move
							xl += stepLightVector;
						}
						// Assign computed light to voxel
						voxelLight = voxelData.m_lightBuffer[voxelID]= Tlight;
					}

					// From here light value and density are correctly set for this voxel
					double Ttemp = exp(-k*stepsize*voxelDensity); // density*100????
					T *= Ttemp;

					//Compute pixel color
					C[0] += m_boxcolor[0] * (1-Ttemp)/k *lightColor[0]*voxelLight*T;
					C[1] += m_boxcolor[1] * (1-Ttemp)/k *lightColor[1]*voxelLight*T;
					C[2] += m_boxcolor[2] * (1-Ttemp)/k *lightColor[2]*voxelLight*T;

					//Move
					xi+=stepVector;
				}

				RGBApixel PixelColor; 
				PixelColor.Red = (ebmpBYTE) std::min(unsigned int((C[0] + m_bgcolor[0] * T)*255), unsigned int(255));
				PixelColor.Green = (ebmpBYTE) std::min(unsigned int((C[1] + m_bgcolor[1] * T)*255), unsigned int(255));
				PixelColor.Blue = (ebmpBYTE) std::min(unsigned int((C[2] + m_bgcolor[2] * T)*255), unsigned int(255));
				PixelColor.Alpha = 0;

				file.SetPixel(i, height-j-1, PixelColor);
			}
		}
		std::cout << "Finished rendering line " << j << std::endl;
	}

	file.WriteToFile(outputFile.c_str());
}