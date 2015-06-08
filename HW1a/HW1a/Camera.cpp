#include "Camera.h"
#include "Config.h"
#include "Vector.h"
#include "EasyBMP.h"
#include <cmath>
#include <algorithm>

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


void Camera::debugRender(unsigned int width, unsigned int height, std::string& outputFile)
{
	BMP file;
	file.SetSize(width,height);
	file.SetBitDepth(24);

	
	double d = m_direction.length();              //Distance from Eye to Middle point of the image

	Vector myVectorEyetoMiddle(m_direction);
	myVectorEyetoMiddle *= d;    //C in full length

	double tanfovx = tan(m_fovRad)*double(width)/height ;  
    Vector myEyeVectorX = myVectorEyetoMiddle.crossProduct(m_up) ;      //A
	Vector myEyeVectorY = myEyeVectorX.crossProduct(myVectorEyetoMiddle) ;      //B

	Vector myMiddlePoint =  myVectorEyetoMiddle + m_position;

	Vector myImageVectorX = myEyeVectorX *  ( myVectorEyetoMiddle.normalize().length() * tanfovx / myEyeVectorX.length() );    //H
	Vector myImageVectorY = myEyeVectorY *  ( myVectorEyetoMiddle.normalize().length() * tan(m_fovRad) / myEyeVectorY.length() );    //V

	for(unsigned int i=0;i<width;i++){
		for(unsigned int j=0;j<height;j++){
			
		    double sx,sy;
			sx = double(i)/width;
			sy = double(j)/height;

			Vector P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;
			Vector myRayCasting = (P - m_position)*(1.0 / (P - m_position).normalize().length()); //RayDirection = (P - E)/normalize(P - E);
		
			Vector normalizedRay = myRayCasting.normalize();

			RGBApixel NewColor;
			NewColor.Red = (ebmpBYTE) abs(normalizedRay[0])*255;
			NewColor.Green = (ebmpBYTE) abs(normalizedRay[1])*255;
			NewColor.Blue = (ebmpBYTE) abs(normalizedRay[2])*255;
			NewColor.Alpha = 0;
			file.SetPixel(i, height-j-1, NewColor);
		}
	}

	file.WriteToFile(outputFile.c_str());
}


bool RayBoxIntersection(double t1x,double t2x, double t1y, double t2y, double t1z,double t2z ) {
	
	double Tnear = -DBL_MAX , Tfar = DBL_MAX;

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

void Camera::renderRayBoxIntersection(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile){

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

	
	RGBApixel BGColor; 
	BGColor.Red = (ebmpBYTE) m_bgcolor[0]*255;
	BGColor.Green = (ebmpBYTE) m_bgcolor[1]*255;
	BGColor.Blue = (ebmpBYTE) m_bgcolor[2]*255;
	BGColor.Alpha = 0;
			
	RGBApixel BoxColor; 
	BoxColor.Red = (ebmpBYTE) m_boxcolor[0]*255;
	BoxColor.Green = (ebmpBYTE) m_boxcolor[1]*255;
	BoxColor.Blue = (ebmpBYTE) m_boxcolor[2]*255;
	BoxColor.Alpha = 0;

	double UnitCube[2][3] = { {0, 0, -1},  {1, 1, 0} };  //cube

	double a1 = UnitCube[0][0] - m_position[0];
	double a2 = UnitCube[1][0] - m_position[0];
	double b1 = UnitCube[0][1] - m_position[1];
	double b2 = UnitCube[1][1] - m_position[1];
	double c1 = UnitCube[0][2] - m_position[2];
	double c2 = UnitCube[1][2] - m_position[2];

	for(unsigned int i=0;i<width;i++){
		for(unsigned int j=0;j<height;j++){


			
			double t1x, t2x, t1y, t2y, t1z, t2z;
		    double sx,sy;
			sx = double(i)/width;
			sy = double(j)/height;

			Vector P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;
			Vector myRayCasting = (P - m_position)*(1.0 / (P - m_position).normalize().length()); //RayDirection = (P - E)/normalize(P - E);
		
			t1x =  a1 / myRayCasting[0];
			t2x =  a2 / myRayCasting[0];
			t1y =  b1 / myRayCasting[1];
			t2y =  b2 / myRayCasting[1];
			t1z =  c1 / myRayCasting[2];
			t2z =  c2 / myRayCasting[2];
			
			bool hit = RayBoxIntersection( t1x, t2x, t1y, t2y, t1z, t2z );

			if (!hit)
				file.SetPixel(i, height-j-1, BGColor);
			else
				file.SetPixel(i, height-j-1, BoxColor);
		}
	}

	file.WriteToFile(outputFile.c_str());
}


void Camera::MoveBox(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], Vector m_newPosition, std::string& outputFile){

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

	
	RGBApixel BGColor; 
	BGColor.Red = (ebmpBYTE) m_bgcolor[0]*255;
	BGColor.Green = (ebmpBYTE) m_bgcolor[1]*255;
	BGColor.Blue = (ebmpBYTE) m_bgcolor[2]*255;
	BGColor.Alpha = 0;
			
	RGBApixel BoxColor; 
	BoxColor.Red = (ebmpBYTE) m_boxcolor[0]*255;
	BoxColor.Green = (ebmpBYTE) m_boxcolor[1]*255;
	BoxColor.Blue = (ebmpBYTE) m_boxcolor[2]*255;
	BoxColor.Alpha = 0;

	double UnitCube[2][3] = { {0, 0, -1},  {1, 1, 0} };  //cube

	//move box
	UnitCube[0][0] += m_newPosition[0];
	UnitCube[1][0] += m_newPosition[0];
    UnitCube[0][1] += m_newPosition[1];
	UnitCube[1][1] += m_newPosition[1];
	UnitCube[0][2] += m_newPosition[2];
	UnitCube[1][2] += m_newPosition[2];

	double a1 = UnitCube[0][0] - m_position[0];
	double a2 = UnitCube[1][0] - m_position[0];
	double b1 = UnitCube[0][1] - m_position[1];
	double b2 = UnitCube[1][1] - m_position[1];
	double c1 = UnitCube[0][2] - m_position[2];
	double c2 = UnitCube[1][2] - m_position[2];

	for(unsigned int i=0;i<width;i++){
		for(unsigned int j=0;j<height;j++){


			
			double t1x, t2x, t1y, t2y, t1z, t2z;
		    double sx,sy;
			sx = double(i)/width;
			sy = double(j)/height;

			Vector P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;
			Vector myRayCasting = (P - m_position)*(1.0 / (P - m_position).normalize().length()); //RayDirection = (P - E)/normalize(P - E);
		
			t1x =  a1 / myRayCasting[0];
			t2x =  a2 / myRayCasting[0];
			t1y =  b1 / myRayCasting[1];
			t2y =  b2 / myRayCasting[1];
			t1z =  c1 / myRayCasting[2];
			t2z =  c2 / myRayCasting[2];
			
			bool hit = RayBoxIntersection( t1x, t2x, t1y, t2y, t1z, t2z );

			if (!hit)
				file.SetPixel(i, height-j-1, BGColor);
			else
				file.SetPixel(i, height-j-1, BoxColor);
		}
	}

	file.WriteToFile(outputFile.c_str());
}