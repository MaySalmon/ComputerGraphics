#include "Material.h"

Material::Material()
{
}

glm::vec3 Material::GetAmbient() const
{
	return ambient_;
}

glm::vec3 Material::GetDiffuse() const
{
	return diffuse_;
}

glm::vec3 Material::GetSpecular() const
{
	return specular_;
}

void Material::SetAmbient(glm::vec3 ambient)
{
	ambient_ = ambient;
}

void Material::SetDiffuse(glm::vec3 diffuse)
{
	diffuse_ = diffuse;
}

void Material::SetSpecular(glm::vec3 specular)
{
	specular_ = specular;
}