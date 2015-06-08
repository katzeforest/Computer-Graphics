#include "Material.h"

Material::Material(const glm::vec3 &diffuseColor, const glm::vec3 & reflectiveColor, float specularExponent, float refractionIndex, bool mirror, bool trans, bool emit)
	: m_diffuseColor(diffuseColor), m_reflectiveColor(reflectiveColor), m_specularExponent(specularExponent), m_refractionIndex(refractionIndex), m_mirror(mirror), m_trans(trans), m_emit(emit)
{}