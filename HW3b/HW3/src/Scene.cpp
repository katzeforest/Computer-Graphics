#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glew/glew.h"

#include "Scene.h"
#include "EasyBMP.h"
#include "Material.h"
#include <cmath>
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
#define EPSILON 1E-5f

glm::vec3 Scene::TraceRay(Ray &ray, unsigned int depth)
{
    static const unsigned int maxDepth = 4;
	//std::cout << "TraceRay origin(" << ray.orig[0] << ", " << ray.orig[1] << ", "  << ray.orig[2] << "), direction(" << ray.dir[0] << ", "  << ray.dir[1] << ", "  << ray.dir[2] << ") Depth=" << depth << std::endl;
	if(depth > maxDepth)
	{
		// Too deep
	    return glm::vec3(0, 0, 0);
	}

	Config * myConfig = Config::getInstance();

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
	
	float * lightColor = myConfig->getLightColor();
	glm::vec3 lightColorVec = glm::vec3(lightColor[0],lightColor[1],lightColor[2]);
	
	    //glm::normalize(    (-objectMaterial->getRefractionIndex() * (normalizedNormal * ray.dir) - sqrt(1 - objectMaterial->getRefractionIndex()*objectMaterial->getRefractionIndex() *(1 - glm::dot(normalizedNormal, ray.dir))* glm::dot(normalizedNormal, ray.dir)))* normalizedNormal + objectMaterial->getRefractionIndex() * ray.dir    ); // surface normal
	glm::vec3 myLightColor = glm::vec3( lightColor[0], lightColor[1], lightColor[2]); 
	glm::vec3 myCameraPosition = myConfig->getCameraPosition();
	glm::vec3 lightvector = glm::normalize(inter.intersectionPoint - glm::vec3(myConfig->getLightPosition()));
	
	float Kd = glm::dot(normalizedNormal, -lightvector);
	glm::vec3 diffuseTerm = myLightColor * Kd * objectMaterial->getDiffuseColor();
	glm::vec3 specTerm;
	glm::vec3 reflectedColor;
	glm::vec3 refractedColor;
	glm::vec3 spec;
	glm::vec3 refr;
	float n = 10;
	glm::vec3 lightreflect = glm::normalize(lightvector + glm::normalize(inter.normal));
	float cos = glm::dot(lightreflect , glm::normalize(inter.normal) );
	Ray reflectedRay;
	Ray refractedRay;
	
	
	glm::vec3 reflectedDir =glm::normalize( -ray.dir - (float) 2 * normalizedNormal ); 

	
	glm::vec3 color(Ka * objectMaterial->getDiffuseColor());

	if(Ks > 0)
	{
		reflectedRay.orig = inter.intersectionPoint - glm::vec3(0.1, 0.1, 0.1) * ray.dir;
		reflectedRay.dir = glm::reflect(ray.dir, normalizedNormal);
		reflectedColor = TraceRay(reflectedRay, depth + 1);  //Ray(inter.intersectionPoint - EPSILON * reflectedDir, reflectedDir)
		spec = (float)0.75 * reflectedColor * objectMaterial->getReflectiveColor();

	}
	else 
		spec = glm::vec3(0, 0, 0); //pow(cos, n) * lightvector ;
		//spec = pow(cos, n);
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
	else
		refr  = glm::vec3(0, 0, 0);

	color = objectMaterial->getDiffuseColor() * (float)0.2 + refr;   

	if((root_->intersect(glm::mat4(), Ray(glm::vec3(myConfig ->getLightPosition()), lightvector)).intersectedObject == inter.intersectedObject))// From point to light check if blocked. From inter point to the light source, remember to stop counting when reach the light source and make sure it is not hitting itself
	{

		//glm::vec3 temp = reflectedDir * myCameraPosition;  //ReflectedRay(lightColor, normal, IntersectionPoint) returns reflected direction
		
		//diffuseTerm =  float(0.5) * objectMaterial ->getDiffuseColor () * glm::normalize(lightColorVec);
		if(glm::length(diffuseTerm) < 0) diffuseTerm = glm::vec3(0,0,0);

		//glm::vec3 reflectedDir = glm::normalize(lightvector + 2 * glm::dot(-lightvector,normalizedNormal) * normalizedNormal); //Ray reflected direction
				
		if(objectMaterial->getMirror() == 0)
		{
			float specCondition = glm::dot(reflectedDir, lightvector);
			if(specCondition < 0)
				specCondition = 0;
			specTerm = float(0.7) * objectMaterial->getReflectiveColor() * pow(specCondition, objectMaterial->getSpecularExponent());
			specTerm[0] = max(0.0f, specTerm[0]);
		    specTerm[1] = max(0.0f, specTerm[1]);
		    specTerm[2] = max(0.0f, specTerm[2]);

		    specTerm[0] = min(1.0f, specTerm[0]);
		    specTerm[1] = min(1.0f, specTerm[1]);
		    specTerm[2] = min(1.0f, specTerm[2]);
		}
		else
			specTerm = spec;

		color += diffuseTerm + specTerm; 

	}
	else if(objectMaterial->getMirror() == 1)
		color += spec;

	if(glm::length(color) < 0) color = glm::vec3(0, 0, 0);
	
	color[0] = min(1.0f, color[0]);
	color[1] = min(1.0f, color[1]);
	color[2] = min(1.0f, color[2]);

	return color;

}

#define M_PI 3.14159265358979323846

void Scene::rayCast()
{
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


	for(unsigned int i=0;i<m_width;i++){
		for(unsigned int j=0;j<m_height;j++){
	//unsigned int i = 400, j = 395;
		    float sx,sy;
			sx = float(i)/m_width;
			sy = float(j)/m_height;

			glm::vec3 P =  myImageVectorX*(2*sx-1) + myImageVectorY*(2*sy-1) + myMiddlePoint ;
			glm::vec3 normalizedRayCast = glm::normalize(P - m_position);

			//Make a ray from the vector
			Ray r(m_position, normalizedRayCast);

			RGBApixel FinalColor;
			
			//FinalColor = (diffuseTerm + ambientTerm) * diffuseColor.rgb + specularTerm * u_LightColor;

			glm::vec3 temp = TraceRay(r, 0);
		
			FinalColor.Red = (ebmpBYTE) (std::max(temp[0],0.0f)*255);
			FinalColor.Green = (ebmpBYTE) (std::max(temp[1],0.0f)*255);
			FinalColor.Blue = (ebmpBYTE) (std::max(temp[2],0.0f)*255);
			FinalColor.Alpha = 0;

			file.SetPixel(i, m_height-j-1, FinalColor);
		}
		std::cout << "Alive i=" << i << " / " << m_width << std::endl;
	}
	file.WriteToFile("raycast.bmp");
}