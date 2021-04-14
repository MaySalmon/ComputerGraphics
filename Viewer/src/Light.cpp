#include "Light.h"
#include <fmt/format.h>

int Light::count = 0;

Light::Light()
	: name_(fmt::format("light_{:02d}", count++))
	, type_(LightType::Directional)
	, light_source_(10.0f, 10.0f, 0.0f)
	, shininess_(200.0f)
	, ambient_(0.0f, 0.0f, 0.0f)
	, diffuse_(149.0f / 255.0f, 66.0f / 255.0f, 66.0f / 255.0f)
	, specular_(1.0f, 1.0f, 1.0f)
{

}

const std::string& Light::GetName() const
{
	return name_;
}

glm::vec3 Light::GetLightSource() const
{
	return light_source_;
}

void Light::SetLightSource(glm::vec3 light_source)
{
	light_source_ = light_source;
}

glm::vec3 Light::GetAmbient() const
{
	return ambient_;
}

glm::vec3 Light::GetDiffuse() const
{
	return diffuse_;
}

glm::vec3 Light::GetSpecular() const
{
	return specular_;
}

void Light::SetAmbient(glm::vec3 ambient)
{
	ambient_ = ambient;
}

void Light::SetDiffuse(glm::vec3 diffuse)
{
	diffuse_ = diffuse;
}

void Light::SetSpecular(glm::vec3 specular)
{
	specular_ = specular;
}