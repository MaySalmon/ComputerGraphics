#include "Scene.h"
#include "MeshModel.h"
#include "Light.h"
#include <string>

Scene::Scene() :
	active_camera_index_(0),
	active_model_index_(0),
	active_light_index_(0)
{

}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models_.push_back(mesh_model);
}

int Scene::GetModelCount() const
{
	return mesh_models_.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models_[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models_[active_model_index_];
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras_.push_back(camera);
}

int Scene::GetCameraCount() const
{
	return cameras_.size();
}

Camera& Scene::GetCamera(int index) const
{
	return *cameras_[index];
}

Camera& Scene::GetActiveCamera() const
{
	return *cameras_[active_camera_index_];
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights_.push_back(light);
}

int Scene::GetLightCount() const
{
	return lights_.size();
}

Light& Scene::GetLight(int index) const
{
	return *lights_[index];
}

Light& Scene::GetActiveLight() const
{
	return *lights_[active_light_index_];
}

void Scene::SetActiveLightIndex(int index)
{
	active_light_index_ = index;
}

int Scene::GetActiveLightIndex() const
{
	return active_light_index_;
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index_ = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index_;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index_ = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index_;
}

void Scene::SetNormalSize(float size)
{
	normal_size_ = size;
}

float Scene::GetNormalSize() const
{
	return normal_size_;
}

void Scene::SetHideNotSelectedModels(bool hide_not_selected)
{
	hide_not_selected_ = hide_not_selected;
}

bool Scene::GetHideNotSelectedModels() const
{
	return hide_not_selected_;
}

void Scene::SetHideNormals(bool hide_normals)
{
	hide_normals_ = hide_normals;
}

bool Scene::GetHideNormals() const
{
	return hide_normals_;
}

void Scene::SetHideBox(bool hide_box)
{
	hide_box_ = hide_box;
}

bool Scene::GetHideBox() const
{
	return hide_box_;
}

bool Scene::GetBlur() const
{
	return blur_;
}



void Scene::SetBlur(bool blur)
{
	blur_ = blur;
}
