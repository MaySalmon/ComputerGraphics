#pragma once
#include <glm/glm.hpp>
#include <string>

class Camera
{
public:
	Camera();
	virtual ~Camera();

	const std::string& GetName() const;
	
	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);

	const glm::mat4x4& GetProjectionTransformation(float aspect) const;
	const glm::mat4x4& GetViewTransformation() const;
	
	void SetPan(float pan) { pan_ = pan; }
	float GetPan() const { return pan_; }
	void SetDolly(float dolly) { dolly_ = dolly; }
	float GetDolly() const { return dolly_; }
	void SetSpin(float spin) { spin_ = spin; }
	float GetSpin() const { return spin_; }
	void SetOrbit(float orbit) { orbit_ = orbit; }
	float GetOrbit() const { return orbit_; }
	void SetZoom(float zoom) { zoom_ = zoom; }
	float GetZoom() const { return zoom_; }
	
private:
	static int count;
	std::string name_;

	glm::mat4x4 view_transformation_;
	glm::mat4x4 projection_transformation_;

	float pan_;
	float dolly_;
	float spin_;
	float orbit_;
	float zoom_;

};
