#include "Cylinder.h"

static const float PI = 3.141592653589f;

// Creates a unit cylinder centered at (0, 0, 0)
Cylinder::Cylinder() :
    Geometry(CYLINDER),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(0.5f),
    height_(1.0f)
{
    buildGeomtery();
}

Cylinder::~Cylinder() {}

void Cylinder::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    unsigned short subdiv = 20;
    float dtheta = 2 * PI / subdiv;

    glm::vec4 point_top(0.0f, 0.5f * height_, radius_, 1.0f),
        point_bottom (0.0f, -0.5f * height_, radius_, 1.0f);
    vector<glm::vec3> cap_top, cap_bottom;

    // top and bottom cap vertices
    for (int i = 0; i < subdiv + 1; ++i) {
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), i * dtheta, glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), center_);

        cap_top.push_back(glm::vec3(translate * rotate * point_top));
        cap_bottom.push_back(glm::vec3(translate * rotate * point_bottom));
    }

    //Create top cap.
    for ( int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_top[0]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_top[i + 2]);
    }
    //Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_bottom[0]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i + 2]);
    }
    //Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i]);
        //Upside-down triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_bottom[i + 1]);
    }

    // create normals
    glm::vec3 top_centerpoint(0.0f , 0.5f * height_ , 0.0f),
        bottom_centerpoint(0.0f, -0.5f * height_, 0.0f);
    glm::vec3 normal(0, 1, 0);

    // Create top cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(normal);
        normals_.push_back(normal);
        normals_.push_back(normal);
    }
    // Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(-normal);
        normals_.push_back(-normal);
        normals_.push_back(-normal);
    }

    // Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i] - bottom_centerpoint));
        //Upside-down triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_top[i + 1] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
    }

    // indices and colors
    glm::vec3 color (0.6f, 0.6f, 0.6f);
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        colors_.push_back(color);
    }

    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        indices_.push_back(i);
    }
}

Intersection Cylinder::intersectImpl(const Ray &ray) const
{
    Intersection inter;

    double a = ray.dir.x*ray.dir.x + ray.dir.z*ray.dir.z;
    double b = 2*(ray.orig.x*ray.dir.x + ray.orig.z*ray.dir.z);
    double c = ray.orig.x*ray.orig.x + ray.orig.z*ray.orig.z - 0.25;
    double deta = b*b - 4*a*c;
    if(ray.dir.x == 0 && ray.dir.z == 0)
    {
        if(abs(c+0.25)>0.25)
			inter.t = -1;
        else
        {
            if(ray.orig.y>0.5)
            {
                inter.t = (0.5 - ray.orig.y)/ray.dir.y;
                inter.normal = glm::vec3(0,1,0);
            }
            else if(ray.orig.y<-0.5)
            {
                inter.t = (-0.5 - ray.orig.y)/ray.dir.y;
                inter.normal = glm::vec3(0,-1,0);
            }
            else
				inter.t = -1;
        }
    }
    else
    {
        if(deta<0)
			inter.t = -1;
        else
        {
            double t1 = (-b - sqrt(deta))/(2*a);
            double t2 = (-b + sqrt(deta))/(2*a);

            if(t1>0)
            {
                double y1 = ray.orig.y + ray.dir.y * t1;
                double y2 = ray.orig.y + ray.dir.y * t2;

                if(y1<0.5 && y1>-0.5)
                {
                    inter.t = t1;
                    inter.normal = glm::normalize(glm::vec3(t1*ray.dir.x+ray.orig.x, 0, t1*ray.dir.z+ray.orig.z));
                }
                else if(y2<=0.5 && y2>=-0.5)
                {
                    float flag;
                    if(y1 >= 0.5)
						flag = 0.5;

                    if(y1 <= -0.5)
						flag = -0.5;

                    float t3 = (flag - ray.orig.y)/ray.dir.y;

                    inter.t = t3;
                    inter.normal = glm::vec3(0, 2*flag, 0);
                }
                else
                {
                    if(y1*y2>0)
                    inter.t = -1;
                    else
                    {
                        float t3 = (0.5 - ray.orig.y)/ray.dir.y;
                        float t4 = (-0.5 - ray.orig.y)/ray.dir.y;

                        if(t3<t4)
                        {
                            inter.t = t3;
                            inter.normal = glm::vec3(0,1,0);
                        }
                        else
                        {
                            inter.t = t4;
                            inter.normal = glm::vec3(0,-1,0);
                        }
                    }
                }
            }
            else if(t2>0)
            {
                float t3;
                double y2 = ray.orig.y + ray.dir.y * t2;

                if(y2>=-0.5 && y2<=0.5)
                {
                    t3 = t2;
                    inter.normal = glm::normalize(glm::vec3(t2*ray.dir.x+ray.orig.x, 0, t2*ray.dir.z+ray.orig.z));
                }
                else if(y2>0.5 && ray.orig.y<0.5)
                {
                    t3 = (0.5 - ray.orig.y)/ray.dir.y;
                    inter.normal = glm::vec3(0,1,0);
                }
                else if(y2<-0.5 && ray.orig.y>-0.5)
                {
                    t3 = (-0.5 - ray.orig.y)/ray.dir.y;
                    inter.normal = glm::vec3(0,-1,0);
                }
                else
					t3 = -1;

                inter.t = t3;
            }
            else
                inter.t = -1;
        }
    }
    return inter;
}