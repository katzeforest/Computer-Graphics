// NOTE: This definition forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
// YOU SHOULD USE THIS IN ALL FILES YOU CREATE WHICH INCLUDE GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glew/glew.h"
#include <GL/glut.h>

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "Geometry.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Config.h"

#include "tests.h"

static const float PI = 3.141592653589f;

// Vertex arrays needed for drawing

unsigned int vboPos;
unsigned int vboCol;
unsigned int vboNor;
unsigned int vboIdx;

// Attributes
unsigned int locationPos;
unsigned int locationCol;
unsigned int locationNor;

// Uniforms
unsigned int unifModel;
unsigned int unifModelInvTr;
unsigned int unifViewProj;
unsigned int unifLightPos;
unsigned int unifCameraPos;
float unifLightColor;

// Needed to compile and link and use the shaders
unsigned int shaderProgram;

// Window dimensions, change if you want a bigger or smaller window
unsigned int windowWidth = 640;
unsigned int windowHeight = 480;

// Animation/transformation stuff
clock_t old_time;
float rotation = 0.0f;

Config myConfig;

// Helper function to read shader source and put it in a char array
// thanks to Swiftless
std::string textFileRead(const char*);

// Some other helper functions from CIS 565 and CIS 277
void printLinkInfoLog(int);
void printShaderInfoLog(int);
void printGLErrorLog();

// Standard glut-based program functions
void init(Scene &scene);
void resize(int, int);
void display(void);
void keypress(unsigned char, int, int);
void mousepress(int button, int state, int x, int y);
void cleanup(void);

void initShader();
void cleanupShader();

Node * selectedNode = NULL;
glm::vec3 selectionColor(1,1,1);

int main(int argc, char** argv)
{
	//FILE *stream ;
	//if((stream = freopen("ouput.txt", "w", stdout)) == NULL)
	//	exit(-1);

	// Check parameters
    if (argc < 2)
	{
		RunTests();
		getchar();
		return 0;
	}

	//Reading config
    if (!myConfig.init(argv[1]))
	{
        std::cerr << "Invalid config file" << std::endl;

		return -1;
	}

	windowWidth = myConfig.getResolution()[0];
	windowHeight = myConfig.getResolution()[1];

	// Select root by default
	selectedNode = myConfig.getScene().getRoot();
	if (selectedNode != NULL)
		selectedNode->setColor(selectionColor);

    glutInit(&argc, argv);
    // Use RGBA double buffered window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Scene Graph");

    glewInit();

    init(myConfig.getScene());
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keypress);
    glutMouseFunc(mousepress);
    glutIdleFunc(display);

    glutMainLoop();

    return 0;
}

void init(Scene &scene)
{
    // Create the VBOs and vboIdx we'll be using to render images in OpenGL
    glGenBuffers(1, &vboPos);
    glGenBuffers(1, &vboCol);
    glGenBuffers(1, &vboNor);
    glGenBuffers(1, &vboIdx);

    // Set the color which clears the screen between frames
    glClearColor(0, 0, 0, 1);
    // Enable and clear the depth buffer
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    // Set up our shaders here
    initShader();

	scene.uploadToBuffers(vboPos, vboCol, vboNor, vboIdx);

    resize(windowWidth, windowHeight);
    old_time = clock();
}

void initShader()
{
    // Read in the shader program source files
    std::string vertSourceS = textFileRead("shaders/diff.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = textFileRead("shaders/diff.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadFrag = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadVert, 1, &vertSource, NULL);
    glCompileShader   (shadVert);
    printShaderInfoLog(shadVert);
    // - Diffuse fragment shader
    glShaderSource    (shadFrag, 1, &fragSource, NULL);
    glCompileShader   (shadFrag);
    printShaderInfoLog(shadFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (shaderProgram, shadVert);
    glAttachShader  (shaderProgram, shadFrag);
    glLinkProgram   (shaderProgram);
    printLinkInfoLog(shaderProgram);

    // Clean up the shaders now that they are linked
    glDetachShader(shaderProgram, shadVert);
    glDetachShader(shaderProgram, shadFrag);
    glDeleteShader(shadVert);
    glDeleteShader(shadFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    locationPos    = glGetAttribLocation (shaderProgram, "vs_Position");
    locationNor    = glGetAttribLocation (shaderProgram, "vs_Normal");
    locationCol    = glGetAttribLocation (shaderProgram, "vs_Color");
    unifViewProj   = glGetUniformLocation(shaderProgram, "u_ViewProj");
    unifModel      = glGetUniformLocation(shaderProgram, "u_Model");
    unifModelInvTr = glGetUniformLocation(shaderProgram, "u_ModelInvTr");
	unifLightPos   = glGetUniformLocation(shaderProgram, "u_LightPos");
	unifLightColor = glGetUniformLocation(shaderProgram, "u_LightColor");
	unifCameraPos  = glGetUniformLocation(shaderProgram, "u_CameraPos");

    printGLErrorLog();
}

void cleanup()
{
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboCol);
    glDeleteBuffers(1, &vboNor);
    glDeleteBuffers(1, &vboIdx);

    glDeleteProgram(shaderProgram);
}

void keypress(unsigned char key, int x, int y)
{
    switch (key) {

	case 'n':
		if (selectedNode != NULL)
		{
			selectedNode->resetColor();
			selectedNode = myConfig.getScene().getNextNode(selectedNode);
			selectedNode->setColor(selectionColor);
			// Upload update scene infos
			myConfig.getScene().uploadToBuffers(vboPos, vboCol, vboNor, vboIdx);
		}
		break;

	case 'a':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[0] -= 0.5;
		break;
    case 'd':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[0] += 0.5;
		break;
	case 'w':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[1] += 0.5;
		break;
	case 's':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[1] -= 0.5;
		break;
	case 'e':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[2] += 0.5;
		break;
	case 'r':
		if (selectedNode != NULL)
			selectedNode->getTranslationVector()[2] -= 0.5;
		break;
	case 'x':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[0] += 0.5;
		break;
	case 'X':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[0] -= 0.5;
		break;
	case 'y':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[1] += 0.5;
		break;
	case 'Y':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[1] -= 0.5;
		break;
	case 'z':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[2] += 0.5;
		break;
	case 'Z':
		if (selectedNode != NULL)
			selectedNode->getScaleVector()[2] -= 0.5;
		break;
	case 'j':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[0] += 3.1415926/18.0;
		break;
	case 'J':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[0] -= 3.1415926/18.0;
		break;
	case 'k':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[1] += 3.1415926/18.0;
		break;
	case 'K':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[1] -= 3.1415926/18.0;
		break;
	case 'l':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[2] += 3.1415926/18.0;
		break;
	case 'L':
		if (selectedNode != NULL)
			selectedNode->getRotationVector()[2] -= 3.1415926/18.0;
		break;
	case 'f':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[0] += 0.5;
		break;
	case 'F':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[0] -= 0.5;
		break;
	case 'g':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[1] += 0.5;
		break;
	case 'G':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[1] -= 0.5;
		break;
	case 'h':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[2] += 0.5;
		break;
	case 'H':
		if (selectedNode != NULL)
			myConfig.getLightPosition()[2] -= 0.5;
		break;
	// Control light color
	case 'b':
		if (selectedNode != NULL)
			myConfig.getLightColor()[0] -= 0.1;
		break;
	case 'B':
		if (selectedNode != NULL)
			myConfig.getLightColor()[0] += 0.1;
		break;
	case 'c':
		if (selectedNode != NULL)
			myConfig.getLightColor()[1] -= 0.1;
		break;
	case 'C':
		if (selectedNode != NULL)
			myConfig.getLightColor()[1] += 0.1;
		break;
	case 't':
		if (selectedNode != NULL)
			myConfig.getLightColor()[2] -= 0.1;
		break;
	case 'T':
		if (selectedNode != NULL)
			myConfig.getLightColor()[2] += 0.1;
		break;
	case 'u':
		if (selectedNode != NULL)
		{
			Node * nodeToDelete = selectedNode;
			
			nodeToDelete->deleteAllChildren();

			if (nodeToDelete == myConfig.getScene().getRoot())
			{
				delete nodeToDelete;
				myConfig.getScene().setRoot(NULL);
				selectedNode = NULL;
			}
			else
			{
				selectedNode = myConfig.getScene().getNextNode(nodeToDelete); // No more children forced to find next node somewhere else
				nodeToDelete->getParent()->deleteChild(nodeToDelete);
			}

			// Upload update scene infos
			myConfig.getScene().uploadToBuffers(vboPos, vboCol, vboNor, vboIdx);
		}
		break;
    case 'q':
        cleanup();
        exit(0);
        break;
	case 'p':
		myConfig.getScene().rayCast();
		break;
    }

    glutPostRedisplay();
}

void mousepress(int button, int state, int x, int y)
{
    // Put any mouse events here
}

void display()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clock_t newTime = clock();
    //rotation += 0.75f * (static_cast<float>(newTime - old_time) / static_cast<float>(CLOCKS_PER_SEC));
    old_time = newTime;

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    // The default provided value is an identity matrix; you'll change this.
    glm::mat4 modelmat = glm::mat4();
	//modelmat = glm::rotate(modelmat, rotation, glm::vec3(0, 1, 0));

    // Make sure you're using the right program for rendering
    glUseProgram(shaderProgram);

    // TODO
    // Draw the two components of our scene separately, for your scenegraphs it
    // will help your sanity to do separate draws for each type of primitive
    // geometry, otherwise your VBOs will get very, very complicated fast

	//Set the light
	glUniform4fv(unifLightPos, 1, &myConfig.getLightPosition()[0]);
	glUniform3fv(unifLightColor, 1, &myConfig.getLightColor()[0]);
	glUniform3fv(unifCameraPos, 1, &myConfig.getCameraPosition()[0]);

	myConfig.getScene().drawScene(modelmat, locationPos, locationCol, locationNor, vboPos, vboCol, vboNor, vboIdx, unifModel, unifModelInvTr);

    // Move the rendering we just made onto the screen
    glutSwapBuffers();

    // Check for any GL errors that have happened recently
    printGLErrorLog();
}

void resize(int width, int height)
{
    // Set viewport
    glViewport(0, 0, width, height);

    // Get camera information
    // Add code here if you want to play with camera settings/ make camera interactive.
    glm::mat4 projection = glm::perspective(myConfig.getFieldOfViewHalf() * PI/180, myConfig.getResolution()[0] / (float) myConfig.getResolution()[1], 0.1f, 100.0f);
	glm::mat4 camera = glm::lookAt(myConfig.getCameraPosition(), myConfig.getCameraPosition() + myConfig.getCameraHeading(), myConfig.getUpVector());
    projection = projection * camera;

    // Upload the projection matrix, which changes only when the screen or
    // camera changes
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &projection[0][0]);

    glutPostRedisplay();
}


std::string textFileRead(const char *filename)
{
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cerr << "Error reading file" << std::endl;
        throw (errno);
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void printGLErrorLog()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char *e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            "unknown";
        std::cerr << e << std::endl;

        // Throwing here allows us to use the debugger stack trace to track
        // down the error.
#ifndef __APPLE__
        // But don't do this on OS X. It might cause a premature crash.
        // http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
        throw;
#endif
    }
}

void printLinkInfoLog(int prog)
{
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void printShaderInfoLog(int shader)
{
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}
