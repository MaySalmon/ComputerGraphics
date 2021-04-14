#pragma once
#include <glm/glm.hpp>
#include <string>

class Material
{
	
public:
	Material();

	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;

	void SetAmbient(glm::vec3 ambient);
	void SetDiffuse(glm::vec3 diffuse);
	void SetSpecular(glm::vec3 specular);

	float GetShininess() const { return shininess_; }
	void SetShininess(float shininess) { shininess_ = shininess; }

private:
	float shininess_=200.0f;
	glm::vec3 ambient_= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 diffuse_ = glm::vec3(149.0f / 255.0f, 66.0f / 255.0f, 66.0f / 255.0f);
	glm::vec3 specular_= glm::vec3(1.0f, 1.0f, 1.0f);
};

