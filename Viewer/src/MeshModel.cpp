#include "MeshModel.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces_(faces),
	vertices_(vertices),
	normals_(normals),
	model_name_(model_name),
	model_translation(0.0f, 0.0f, 0.0f),
	model_scale(1.0f, 1.0f, 1.0f),
	model_rotation(0.0f, 1.0f, 1.0f)
{

}

MeshModel::~MeshModel()
{
}

const Face& MeshModel::GetFace(int index) const
{
	return faces_[index];
}

int MeshModel::GetFacesCount() const
{
	return faces_.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name_;
}

const glm::vec3& MeshModel::GetVertex(int index) const
{
	return vertices_[index];
}

const glm::vec3& MeshModel::GetNormal(int index) const
{
	return normals_[index];
}
