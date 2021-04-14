#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>
#include "Face.h"
#include "Material.h"


class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;

	const glm::vec3& GetVertex(int index) const;
	const glm::vec3& GetNormal(int index) const;

	const glm::vec3& GetTranslation() const { return model_translation; }
	const glm::vec3& GetScale() const { return model_scale; }
	const glm::vec3& GetRotation() const { return model_rotation; }

	void SetTranslation(const glm::vec3 translation) { model_translation = translation; }
	void SetScale(const glm::vec3 scale) { model_scale = scale; }
	void SetRotation(const glm::vec3 rotation) { model_rotation = rotation; }

	Material& GetMaterial() { return matrial_; }
	const Material& GetMaterial () const { return matrial_; }

private:

	std::vector<Face> faces_;
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::string model_name_;
	glm::vec3 model_translation;
	glm::vec3 model_scale;
	glm::vec3 model_rotation;
	Material matrial_;
};
