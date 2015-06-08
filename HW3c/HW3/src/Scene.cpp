#define _USE_MATH_DEFINES
#include <cmath>
#include <cfloat>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "glew/glew.h"

#include "Scene.h"
#include "EasyBMP.h"
#include "Material.h"
#include <ctime>
#include <algorithm>

#include "Intersection.h"

Scene::Scene()
{
	root_ = NULL;
}

Scene::~Scene() {
	if (root_)
		delete root_;
	root_ = NULL;
}

void Scene::setRoot(Node * root)
{
	root_ = root;
}

Node * Scene::getRoot()
{
	return root_;
}

void Scene::indexLightNodes()
{
	if (!root_)
		return;

	for (unsigned int i=0; i<root_->getChildrenCount(); ++i)
	{
		Node * child = root_->getChild(i);
		if (child && child->getMaterial()->getEmit())
			lightNodes_.push_back(child);
	}
}

Node * Scene::getNodeFromName(std::string& name)
{
	if (root_ != NULL)
		return root_->findChildByName(name);
	else
		return NULL;
}

Node * Scene::getNextNode(Node * start)
{
	if (start->getChildrenCount() > 0)
		return start->getChild(0);
	else
	{
		Node * currentNode = start;
		while (currentNode != root_)
		{
			//Find if there is a next child for parent
			Node * parentNode = currentNode->getParent();
			for (unsigned int i=0; i < parentNode->getChildrenCount(); ++i)
			{
				Node * child = parentNode->getChild(i);
				if (child == currentNode)
				{
					if (i+1 < parentNode->getChildrenCount())
					{
						//Move to next child
						return parentNode->getChild(i+1);
					}
				}
			}

			//else we go parent node
			currentNode = parentNode;
		}

		return root_; 
	}
}


void Scene::uploadToBuffers(unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx)
{
	static const GLsizei SIZE_POS = sizeof(glm::vec3);
	static const GLsizei SIZE_NOR = sizeof(glm::vec3);
	static const GLsizei SIZE_COL = sizeof(glm::vec3);
	static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

	positions_.clear();
	normals_.clear();
	colors_.clear();
	indices_.clear();

	if (root_)
	{
		root_->buildArrays(positions_, normals_, colors_, indices_);
	}

	if (positions_.size() == 0)
		return;

	// ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
	// Now we put the data into the Vertex Buffer Object for the graphics system to use
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	// Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
	// Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
	// Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &positions_[0], GL_STATIC_DRAW);

	// Bind+upload the color data
	glBindBuffer(GL_ARRAY_BUFFER, vboCol);
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &colors_[0], GL_STATIC_DRAW);

	// Bind+upload the normals
	glBindBuffer(GL_ARRAY_BUFFER, vboNor);
	glBufferData(GL_ARRAY_BUFFER, positions_.size() * SIZE_POS, &normals_[0], GL_STATIC_DRAW);

	// Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 3 * SIZE_TRI, &indices_[0], GL_STATIC_DRAW);
}

void Scene::drawScene(glm::mat4 model, unsigned int locationPos, unsigned int locationCol, unsigned int locationNor,
	unsigned int vboPos, unsigned int vboCol, unsigned int vboNor, unsigned int vboIdx, unsigned int unifModel, unsigned int unifModelInvTr)
{
	if (root_)
	{
		// =============================== Draw the data that we sent =================================
		// Activate our three kinds of vertex information
		glEnableVertexAttribArray(locationPos);
		glEnableVertexAttribArray(locationCol);
		glEnableVertexAttribArray(locationNor);

		// Tell the GPU where the positions are: in the position buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboPos);
		glVertexAttribPointer(locationPos, 3, GL_FLOAT, false, 0, NULL);

		// Tell the GPU where the colors are: in the color buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboCol);
		glVertexAttribPointer(locationCol, 3, GL_FLOAT, false, 0, NULL);

		// Tell the GPU where the normals are: in the normal buffer (4 components each)
		glBindBuffer(GL_ARRAY_BUFFER, vboNor);
		glVertexAttribPointer(locationNor, 3, GL_FLOAT, false, 0, NULL);

		// recursive draw
		unsigned int offset = 0;
		root_->drawScene(model, unifModel, unifModelInvTr, offset);

		// Shut off the information since we're done drawing.
		glDisableVertexAttribArray(locationPos);
		glDisableVertexAttribArray(locationCol);
		glDisableVertexAttribArray(locationNor);
	}
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

#include "Config.h"

glm::vec3 getCosineWeightedDirection(const glm::vec3& normal);

#define EPSILON 1E-5f

#ifndef LIGHT_DEFINITION
	#define LIGHT_DEFINITION 32
#endif


#ifndef _MC
glm::vec3 Scene::TraceRay(Ray &ray, unsigned int depth)
{
    static const unsigned int maxDepth = 4;
	//std::cout << "TraceRay origin(" << ray.orig[0] << ", " << ray.orig[1] << ", "  << ray.orig[2] << "), direction(" << ray.dir[0] << ", "  << ray.dir[1] << ", "  << ray.dir[2] << ") Depth=" << depth << std::endl;
	if(depth > maxDepth)
	{
		// Too deep
	    return glm::vec3(0, 0, 0);
	}

	// Find intersection point
	glm::mat4 model;
	Intersection inter = root_->intersect(model, ray);

	if (inter.t < 0)
	{
		//std::cout << "\tNo intersection" << std::endl;
		// If parralel to light source set to light color
		if (false)
		{
			return glm::vec3(0, 0, 0); // Light color
		}
		else // else return background color
			return glm::vec3(0, 0, 0);
	}
	
	//std::cout << "\tIntersected with t=" << inter.t << " object=" << inter.intersectedObject->getName() << " normal=(" << inter.normal[0] << ", "  << inter.normal[1] << ", "  << inter.normal[2] << "), interPoint=(" << inter.intersectionPoint[0] << ", "  << inter.intersectionPoint[1] << ", "  << inter.intersectionPoint[2] << ")" << std::endl;
	Material * objectMaterial = inter.intersectedObject->getMaterial();

	float Ks = objectMaterial->getMirror();// Ks is MIRR in material
	float Kt = objectMaterial->getTrans() ? 1.0f : 0.0f;   //Kt is TRANS in material
	float Ka = 0.2; // Ambient light set random number 

	glm::vec3 normalizedNormal = glm::normalize(inter.normal);  //Normal of the intersection point
	
	glm::vec3 diffuseTerm(0, 0, 0);
	glm::vec3 specTerm(0, 0, 0);
	glm::vec3 reflectedColor;
	glm::vec3 refractedColor;
	glm::vec3 refr;
	float n = 10;
	Ray reflectedRay;
	Ray refractedRay;

	glm::vec3 reflectedDir =glm::normalize( -ray.dir - (float) 2 * normalizedNormal ); 
	glm::vec3 color(Ka * objectMaterial->getDiffuseColor());

	if(Ks > 0 && objectMaterial->getMirror())
	{
		reflectedRay.orig = inter.intersectionPoint - glm::vec3(0.1, 0.1, 0.1) * ray.dir;
		reflectedRay.dir = glm::reflect(ray.dir, normalizedNormal);
		reflectedColor = TraceRay(reflectedRay, depth + 1);  //Ray(inter.intersectionPoint - EPSILON * reflectedDir, reflectedDir)
		specTerm = (float)0.75 * reflectedColor * objectMaterial->getReflectiveColor();
	}

	glm::vec3 refractedDir;

    if(Kt > 0)
	{
		
	    float n12 = 1.0/objectMaterial->getRefractionIndex();
		float n21 = objectMaterial->getRefractionIndex();
		float InorOut = glm::dot(inter.normal, ray.dir);
		if(InorOut < 0)
			refractedDir =  glm::refract(ray.dir,normalizedNormal,n12);
		else
			refractedDir =  glm::refract(ray.dir,-normalizedNormal,n21);

		refractedRay.orig = inter.intersectionPoint + (float)0.0008*refractedDir;
		refractedRay.dir = glm::normalize(refractedDir);
		refractedColor = TraceRay(refractedRay, depth+1); //TraceRay(Ray(inter.intersectionPoint + EPSILON * refractedDir, refractedDir), depth + 1);		 
	    refr = float(Kt) * refractedColor ;
	}

	if (!objectMaterial->getEmit())
	{
		for (unsigned int i=0; i < lightNodes_.size(); ++i)
		{
			Node * lightNode = lightNodes_[i];
			glm::vec3 myLightColor = lightNode->getMaterial()->getDiffuseColor();

			for (unsigned int j=0; j < LIGHT_DEFINITION; ++j)
			{
				glm::vec3 lightpoint = lightNode->getRandomPoint();
				glm::vec3 lightvector = glm::normalize(lightpoint - inter.intersectionPoint);

				float Kd = glm::dot(normalizedNormal, lightvector);

				if(root_->intersect(glm::mat4(), Ray(inter.intersectionPoint + (float)0.0008*lightvector, lightvector)).intersectedObject == lightNode)// From point to light check if blocked. From inter point to the light source, remember to stop counting when reach the light source and make sure it is not hitting itself
				{
					diffuseTerm += myLightColor * Kd * objectMaterial->getDiffuseColor();
					//if(glm::length(diffuseTerm) < 0) diffuseTerm = glm::vec3(0,0,0);

					if(!objectMaterial->getMirror())
					{
						float specCondition = glm::dot(reflectedDir, -lightvector);

						if(specCondition < 0)
							specCondition = 0;

						glm::vec3 tempSpecTerm(0.7f * objectMaterial->getReflectiveColor() * pow(specCondition, objectMaterial->getSpecularExponent()));
						specTerm[0] += min(1.0f, max(0.0f, tempSpecTerm[0]));
						specTerm[1] += min(1.0f, max(0.0f, tempSpecTerm[1]));
						specTerm[2] += min(1.0f, max(0.0f, tempSpecTerm[2]));
					}
				}
			}
		}
	}
	else
	{
		diffuseTerm = objectMaterial->getDiffuseColor();
	}

	float lightDivider = (lightNodes_.size() > 0) ? 1.0f/(lightNodes_.size() * LIGHT_DEFINITION) : 1.0f;

	if (!objectMaterial->getMirror())
		specTerm *= lightDivider;
		
	if (!objectMaterial->getEmit())
		diffuseTerm *= lightDivider;

	color = objectMaterial->getDiffuseColor() * (float)0.2 + refr +  specTerm + (diffuseTerm) ;


	//if(glm::length(color) < 0) color = glm::vec3(0, 0, 0);
	
	color[0] = min(1.0f, color[0]);
	color[1] = min(1.0f, color[1]);
	color[2] = min(1.0f, color[2]);

	return color;
}

#else

glm::vec3 Scene::TraceRay(Ray &ray, unsigned int depth, glm::vec3 & MCtrans)
{
    static const unsigned int maxDepth = 5;

	if(depth > maxDepth)
	{
		// Too deep
		//MCtrans = glm::vec3(0.0f, 0.0f, 0.0f);
	    return MCtrans;
	}

	// Find intersection point
	glm::mat4 model;
	Intersection inter = root_->intersect(model, ray);
	//std::cout << "TraceRay origin(" << ray.orig[0] << ", " << ray.orig[1] << ", "  << ray.orig[2] << "), direction(" << ray.dir[0] << ", "  << ray.dir[1] << ", "  << ray.dir[2] << ") Depth=" << depth << std::endl

	if (inter.t < 0)
	{
		MCtrans = glm::vec3(0.0f, 0.0f, 0.0f);
		return MCtrans;
	}
	else
	{
		//std::cout << "\tIntersected with t=" << inter.t << " object=" << inter.intersectedObject->getName() << " normal=(" << inter.normal[0] << ", "  << inter.normal[1] << ", "  << inter.normal[2] << "), interPoint=(" << inter.intersectionPoint[0] << ", "  << inter.intersectionPoint[1] << ", "  << inter.intersectionPoint[2] << ")" << std::endl;
		Material * objectMaterial = inter.intersectedObject->getMaterial();

		float Ks = objectMaterial->getMirror();// Ks is MIRR in material
		float Kt = objectMaterial->getTrans() ? 1.0f : 0.0f;   //Kt is TRANS in material

		glm::vec3 normalizedNormal = glm::normalize(inter.normal);  //Normal of the intersection point

		float n = 10;
		Ray reflectedRay;
		Ray refractedRay;	

		float MaxRGB ;
		float absorbTerm; 

		if (objectMaterial->getEmit())
		{
			MCtrans *= objectMaterial->getDiffuseColor();//glm::vec3(1.0f, 1.0f, 1.0f) * (float)objectMaterial->getEmit();
			return MCtrans;
		}
		else
		{
			if(Ks > 0)
			{
				glm::vec3 reflectedDir =glm::normalize( -ray.dir - (float) 2 * normalizedNormal ); 
				reflectedRay.orig = inter.intersectionPoint - glm::vec3(0.1f, 0.1f, 0.1f) * ray.dir;
				reflectedRay.dir = glm::reflect(ray.dir, normalizedNormal);
				MCtrans = MCtrans * objectMaterial->getReflectiveColor() ;
				TraceRay(reflectedRay, depth + 1, MCtrans);  
				//Ray(inter.intersectionPoint - EPSILON * reflectedDir, reflectedDir);
				//specTerm = (float)0.75 * reflectedColor * objectMaterial->getReflectiveColor();
			}
		    

			else if(Kt > 0)
			{
				glm::vec3 refractedDir;
				float n12 = 1.0/objectMaterial->getRefractionIndex();
				float n21 = objectMaterial->getRefractionIndex();
				float InorOut = glm::dot(inter.normal, ray.dir);
				if(InorOut < 0)
					refractedDir =  glm::refract(ray.dir,normalizedNormal,n12);
				else
					refractedDir =  glm::refract(ray.dir,-normalizedNormal,n21);

				refractedRay.orig = inter.intersectionPoint + (float)0.001*refractedDir;
				refractedRay.dir = glm::normalize(refractedDir);
				MCtrans = MCtrans * glm::vec3(1.0f, 1.0f, 1.0f) ;
				TraceRay(refractedRay, depth + 1, MCtrans); 
			}

			else
			{
				glm::vec3 RandReflect = getCosineWeightedDirection(inter.normal); 
				RandReflect = glm::normalize(RandReflect);
				Ray MCreflect(inter.intersectionPoint, RandReflect);
				MCtrans = MCtrans * objectMaterial->getDiffuseColor();
				MaxRGB = max(objectMaterial->getDiffuseColor()[0], max (objectMaterial->getDiffuseColor()[1], objectMaterial->getDiffuseColor()[2])) ;
				absorbTerm = 1.0f - MaxRGB;
				

				if( rand()%100 < int(absorbTerm*100))
				{
					MCtrans = glm::vec3(0.0f, 0.0f, 0.0f);
					return MCtrans;
				}
				else
				{ 
					if( MaxRGB > FLT_EPSILON)
						MCtrans /= MaxRGB;
					else
						MCtrans = glm::vec3(0.0f, 0.0f, 0.0f);
					glm::vec3 RandReflect = getCosineWeightedDirection(normalizedNormal);
					RandReflect = glm::normalize(RandReflect);
					Ray MCreflect(inter.intersectionPoint, RandReflect);
					TraceRay( MCreflect, depth + 1, MCtrans);
				}
			}

		}
	}
}

#endif

#include <omp.h>

// Activate Anti aliasing by defining symbol _AA
//#define _AA

void Scene::rayCast()
{
	std::clock_t start = std::clock();

	glm::mat4 modelmat = glm::mat4(); // This will be needed to pass down the node hierachy when rendering

	Config * myConfig = Config::getInstance();

	unsigned int m_width = myConfig->getResolution()[0];
	unsigned int m_height = myConfig->getResolution()[1];
	glm::vec3 &m_position = myConfig->getCameraPosition();
	glm::vec3 &m_direction = myConfig->getCameraHeading();
	glm::vec3 &m_up = myConfig->getUpVector();
	unsigned int m_fovY = myConfig->getFieldOfViewHalf();
	float m_fovRad = m_fovY * M_PI/360;

	// Debug rendering
	BMP file;
	file.SetSize(m_width,m_height);
	file.SetBitDepth(24);
	
	double d = glm::length(m_direction);              //Distance from Eye to Middle point of the image

	glm::vec3 myVectorEyetoMiddle(m_direction);
	myVectorEyetoMiddle *= d;    //C in full length

	double tanfovx = tan(m_fovRad)*double(m_width)/m_height ;  
    glm::vec3 myEyeVectorX = glm::cross(myVectorEyetoMiddle, m_up) ;      //A
	glm::vec3 myEyeVectorY = glm::cross(myEyeVectorX, myVectorEyetoMiddle) ;      //B

	glm::vec3 myMiddlePoint =  myVectorEyetoMiddle + m_position;

	glm::vec3 myImageVectorX = myEyeVectorX * float( tanfovx / glm::length(myEyeVectorX) );    //H
	glm::vec3 myImageVectorY = myEyeVectorY * float( tan(m_fovRad) / glm::length(myEyeVectorY) );    //V
	
	glm::mat4 model = glm::mat4(); //identity matrix

#ifdef _AA
			glm::vec3 halfStepX(myImageVectorX * (0.5f/m_width));
			glm::vec3 halfStepY(myImageVectorY * (0.5f/m_width));
#endif

	unsigned int progress = 0;

	#pragma omp parallel for
	for( int j=0;j<(int)m_height;j++){
		for(unsigned int i=0;i<m_width;i++){
		    float sx,sy;
			sx = float(i)/m_width;
			sy = float(j)/m_height;

			glm::vec3 P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;

			RGBApixel pixelColor;
#ifdef _AA
			Ray r1(m_position, glm::normalize(P - halfStepX - halfStepY - m_position));
			Ray r2(m_position, glm::normalize(P - halfStepX + halfStepY - m_position));
			Ray r3(m_position, glm::normalize(P + halfStepX + halfStepY - m_position));
			Ray r4(m_position, glm::normalize(P + halfStepX - halfStepY - m_position));

			glm::vec3 temp = TraceRay(r1, 0) + TraceRay(r2, 0) + TraceRay(r3, 0) + TraceRay(r4, 0);

			const float colorDivider = 0.25f;
#else
			//Make a ray from the vector
			Ray r(m_position, glm::normalize(P - m_position));

#ifndef _MC
			glm::vec3 temp = TraceRay(r, 0);
#else
			int MCnumber = 100; //Times of Monte Carlo algorithm
			glm::vec3 temp(0.0f, 0.0f, 0.0f);
			for(int n = 0; n < MCnumber; n++)
			{
				temp += TraceRay(r, 0, glm::vec3(1.0f, 1.0f, 1.0f));
			}

			temp /= (float)MCnumber;

			temp[0] = std::max(0.0f, std::min(temp[0], 1.0f));
			temp[1] = std::max(0.0f, std::min(temp[1], 1.0f));
			temp[2] = std::max(0.0f, std::min(temp[2], 1.0f));
#endif

			const float colorDivider = 1.0f;
#endif

			pixelColor.Red = (ebmpBYTE) (std::max(temp[0] * colorDivider,0.0f)*255);
			pixelColor.Green = (ebmpBYTE) (std::max(temp[1] * colorDivider,0.0f)*255);
			pixelColor.Blue = (ebmpBYTE) (std::max(temp[2] * colorDivider,0.0f)*255);
			pixelColor.Alpha = 0;

			file.SetPixel(i, m_height-j-1, pixelColor);
		}
		// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
		std::cout << "Progress " << (++progress)/float(m_height) * 100 << "%" << std::endl;
	}

	std::cout << "----- Finished, CPU time=" << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms -----" << std::endl;
	file.WriteToFile("raycast.bmp");
}