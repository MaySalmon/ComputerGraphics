#pragma once
#include <glm/glm.hpp>
#include <string>

class Light
{
public:
	enum class LightType
	{
		Directional = 0,
		Positional = 1
	};
public:
	Light();

	const std::string& GetName() const;

	LightType GetType() const { return type_; }
	void GetType(LightType type) { type_ = type; }

	glm::vec3 GetLightSource() const;
	void SetLightSource(glm::vec3 light_source);

	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;

	void SetAmbient(glm::vec3 ambient);
	void SetDiffuse(glm::vec3 diffuse);
	void SetSpecular(glm::vec3 specular);

	float GetShininess() const { return shininess_; }
	void SetShininess(float shininess) { shininess_ = shininess; }

private:
	static int count;
	std::string name_;
	LightType type_;
	glm::vec3 light_source_;
	float shininess_;
	glm::vec3 ambient_;
	glm::vec3 diffuse_;
	glm::vec3 specular_;
};

