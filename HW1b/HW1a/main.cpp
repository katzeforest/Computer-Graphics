#include <cmath>
#include <iostream>
#include <fstream>
#include "Camera.h"
#include "Config.h"
#include "EasyBMP.h"

using namespace std; 

int main(int argc, char** argv)
{
	// Check parameters
    if (argc < 2)
	{
        std::cerr << "USAGE: " << argv[0] << " configFile.txt" << std::endl;
		return -1;
	}

	// Load config
    Config myConfig;
    if (!myConfig.init(argv[1]))
	{
        std::cerr << "Invalid config file" << std::endl;
		return -1;
	}

	// Output it as the same format than the input file to be able to compare them
	/*std::string testOutput = std::string(argv[1]) + ".out.txt";
	std::ofstream outputFile(testOutput, std::ofstream::out);

	outputFile << myConfig;
	outputFile.close();*/

	//std::cout << "***** READ VALUES *****" << std::endl << myConfig << std::endl;
	//std::cout << "RESOLUTION X=" <<myConfig.getResolution()[0] << "Y=" <<myConfig.getResolution()[1] << std::endl;

	int w, h;
	w = myConfig.getResolution()[0];
	h = myConfig.getResolution()[1];

	VoxelData voxelData(myConfig);

	// here all the voxel info you need are accessible in voxelData, you can pass a reference on it to your new renderer

	Camera myCamera(myConfig.getCameraPosition(),myConfig.getCameraHeading(),myConfig.getUpVector(),myConfig.getFieldOfViewHalf());
	
	myCamera.renderRayMarching(w, h, myConfig.getBackgroundColor(), myConfig.getMaterialColor(), myConfig.getOutputFileName()+"_raymatching.bmp", voxelData, myConfig.getStep(), myConfig.getLightPosition(), myConfig.getLightColor());

	// To keep the console opened
	//getchar();
}




	

	