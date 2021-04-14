#include "Camera.h"
#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

int Camera::count = 0;

Camera::Camera()
	: name_(fmt::format("camera_{:02d}", count++))
	, spin_(0.0f)
	, orbit_(0.0f)
	, pan_(0.0f)
	, dolly_(0.0f)
	, zoom_(60.0f)
{
		
}

Camera::~Camera()
{
	
}

const std::string& Camera::GetName() const
{
	return name_;
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation_ = glm::lookAt(eye, at, up);
}

const glm::mat4x4& Camera::GetProjectionTransformation(float aspect) const
{
	return glm::perspective(glm::radians(zoom_), aspect, 1.0f, 100.0f);
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	return glm::rotate(glm::mat4(1.0f), glm::radians(spin_), glm::vec3{ 0.0f, 1.0f, 0.0f })
		* view_transformation_ \
		* glm::translate(glm::mat4(1.0f), glm::vec3{ -pan_ , 0.0f, -dolly_ }) \
		* glm::rotate(glm::mat4(1.0f), glm::radians(-orbit_), glm::vec3{ 0.0f, 1.0f, 0.0f });
}
