#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Material {
public:
	Material(const glm::vec3& diffuseColor, const glm::vec3& reflectiveColor, float specularExponent, float refractionIndex, bool mirror, bool trans, bool emit);

	glm::vec3& getDiffuseColor()	{ return m_diffuseColor; }
	glm::vec3& getReflectiveColor() { return m_reflectiveColor; }
	float getSpecularExponent()		{ return m_specularExponent; }
	float getRefractionIndex()		{ return m_refractionIndex; }
	bool getMirror()				{ return m_mirror; }
	bool getTrans()					{ return m_trans; }
	bool getEmit()					{ return m_emit; }

private:
	glm::vec3 m_diffuseColor;
	glm::vec3 m_reflectiveColor;
	float m_specularExponent;
	float m_refractionIndex;
	bool m_mirror;
	bool m_trans;
	bool m_emit;
};