How To Run
  Put all the test.txt in the debug default folder. Open HW1a.sln in VS 2010.  Right click the Solution and go to Properties -> Configuration Properties -> Debugging -> Command Arguments to change different test files. Run the project.

Files Includes With This Project:
main.cpp  Vector.cpp  Config.cpp  Camera.cpp  EasyBMP.cpp
          Vector.h    Config.h    Camera.h    EasyBMP headers

Homework # 1a
Requirements:
1. Camera
  The configuration file¡¯s name is taken as a command line argument. The Camera gets information of EYEP, VDIR, UVEC and FOVY from the file directly.

class Camera
{
private:
        Vector                  m_position;
        Vector                  m_direction;
        Vector                  m_up;
        int                             m_fovY; // half fov angle
        double        m_fovRad; // half fov angle in radians

public:
    Camera(Vector& position, Vector& direction, Vector& upVector, int fov);
    ~Camera(void);

        void debugRender(unsigned int width, unsigned int height, std::string& outputFile);
        void renderRayBoxIntersection(unsigned int width, unsigned int height, float m_bgcolor[], float m_boxcolor[], std::string& outputFile);
};

2. Ray Casting
  The Ray Casting Method is in void Camera::debugRender(). It is the same algorithm as in the slides#163. It will output a debug image named ¡°FILE_debug.bmp¡± which FILE is the filename given to the output image. Please check the code for details.

3. Ray-Box Intersection
  The Ray-Box Intersection function is the bool RayBoxIntersection(). If the ray intersects with the box it will return true. The output image is given by void Camera::renderRayBoxIntersection(). If bool RayBoxIntersection() returns 1, the pixel will get the MRGB. Otherwise it will get the BRGB. It will output a debug image named ¡°FILE.bmp¡± which FILE is the filename given to the output image.

Extra Credit:
  Add NPOS data to the test file and it will move the min point of the box by this vector. I set NPOS as 2 -2 2 in test1.txt to get the BabysFirstCloudImage.bmp_movebox.bmp image.