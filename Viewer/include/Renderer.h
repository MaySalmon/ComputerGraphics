#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	struct Vertex
	{
		// a container for all vertex attributes
		glm::vec3 v;       // in camera space
		glm::vec3 pos;     // in raster space
		glm::vec3 normal;  // in camera space
		glm::vec2 txcoord;
		glm::vec2 color;
	};

	enum class ShaderMode
	{
		Gouraud, Phong, Flat
	};

	struct Uniform
	{
		// TODO: add uniforms, such as array of lights, texture index, ambient diffuse and specular color
		ShaderMode mode;
		std::vector<Light*> lights;
		Material* material;
	};

public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void ClearDepthBuffer(float depth);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void SetFog(bool fog);
	bool GetFog() const;
	void SetRenderMode(ShaderMode mode) { shader_mode_ = mode; }
	ShaderMode SetRenderMode() const { return shader_mode_; }

private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void PutDepth(const int i, const int j, float depth);
	glm::vec3 GetPixel(int i, int j)const;
	bool DepthTest(const int i, const int j, float depth);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& color);

	glm::vec4 CalculateFragColor(glm::vec3 pos, glm::vec3 norm, const Uniform& u);
	void ShadeTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Uniform& u);


	void CreateBuffers(int w, int h);
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();

	/* post processing */
	bool HasPixel(int i, int j);
	void FilterCreation();
	void BlurFunction();

	float* color_buffer_;
	float* depth_buffer_;
	int viewport_width_;
	int viewport_height_;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
	ShaderMode shader_mode_;

	int gaussian_kern_width_ = 5;
	int gaussian_kern_height_ = 5;
	float *gaussian_kern_;
	bool fog_;
};
