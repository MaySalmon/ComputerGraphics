#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "Light.h"
#include "MeshModel.h"

class Scene {
public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;

	void AddCamera(const std::shared_ptr<Camera>& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index) const;
	Camera& GetActiveCamera() const;

	void AddLight(const std::shared_ptr<Light>& light);
	int GetLightCount() const;
	Light& GetLight(int index) const;
	Light& GetActiveLight() const;

	void SetActiveLightIndex(int index);
	int GetActiveLightIndex() const;

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;

	void SetNormalSize(float size);
	float GetNormalSize() const;
	void SetHideNotSelectedModels(bool hide_not_selected);
	bool GetHideNotSelectedModels() const;
	void SetHideNormals(bool hide_normals);
	bool GetHideNormals() const;
	void SetHideBox(bool hide_box);
	bool GetHideBox() const;
	void SetBlur(bool blur);
	bool GetBlur() const;
	

private:
	std::vector<std::shared_ptr<MeshModel>> mesh_models_;
	std::vector<std::shared_ptr<Camera>> cameras_;
	std::vector<std::shared_ptr<Light>> lights_;

	//we added some attributes to Scene class
	int active_camera_index_;
	int active_model_index_;
	int active_light_index_;
	float normal_size_;
	bool hide_not_selected_;
	bool hide_normals_;
	bool hide_box_;
	bool blur_;
	
};