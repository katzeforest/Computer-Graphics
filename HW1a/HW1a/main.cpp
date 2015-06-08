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


	Camera myCamera(myConfig.getCameraPosition(),myConfig.getCameraHeading(),myConfig.getUpVector(),myConfig.getFieldOfViewHalf());
	myCamera.debugRender(w, h, myConfig.getOutputFileName()+"_debug.bmp");
	myCamera.renderRayBoxIntersection(w, h, myConfig.getBackgroundColor(), myConfig.getMaterialColor(), myConfig.getOutputFileName());

	if (myConfig.getNewPosition().length() !=0){

	myCamera.MoveBox(w, h, myConfig.getBackgroundColor(), myConfig.getMaterialColor(), myConfig.getNewPosition(), myConfig.getOutputFileName()+"_movebox.bmp");
	}
	// To keep the console opened
	//getchar();
	
}




	

	