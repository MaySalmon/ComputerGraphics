#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <array>

#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))
#define G_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width_(viewport_width),
	viewport_height_(viewport_height),
	shader_mode_(ShaderMode::Phong)
{
	InitOpenGLRendering();
	CreateBuffers(viewport_width, viewport_height);
	gaussian_kern_ = new float[gaussian_kern_width_ * gaussian_kern_height_];

	// precalculate gaussian kernel
	FilterCreation();
}

Renderer::~Renderer()
{
	delete[] color_buffer_;
	delete[] gaussian_kern_;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width_) return;
	if (j < 0) return; if (j >= viewport_height_) return;

	color_buffer_[INDEX(viewport_width_, i, j, 0)] = color.x;
	color_buffer_[INDEX(viewport_width_, i, j, 1)] = color.y;
	color_buffer_[INDEX(viewport_width_, i, j, 2)] = color.z;
}

void Renderer::PutDepth(int i, int j, float depth)
{
	depth_buffer_[Z_INDEX(viewport_width_, i, j)] = depth;
	//color_buffer_[INDEX(viewport_width_, i, j, 0)] = depth;
	//color_buffer_[INDEX(viewport_width_, i, j, 1)] = depth;
	//color_buffer_[INDEX(viewport_width_, i, j, 2)] = depth;
}

glm::vec3 Renderer::GetPixel(int i, int j) const
{
	glm::vec3 PixelColor;
	PixelColor.x = color_buffer_[INDEX(viewport_width_, i, j, 0)];
	PixelColor.y = color_buffer_[INDEX(viewport_width_, i, j, 1)];
	PixelColor.z = color_buffer_[INDEX(viewport_width_, i, j, 2)];
	return PixelColor;
}

bool Renderer::DepthTest(int i, int j, float depth)
{
	return depth_buffer_[Z_INDEX(viewport_width_, i, j)] > depth;
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{

	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	int x1, x2, y1, y2, dx, sx, dy, sy, err1, err2;
	x1 = p1.x; //we set the point (x1,y1) by the first point we get
	y1 = p1.y;
	x2 = p2.x; //we set the point (x2,y2) by the second point we get
	y2 = p2.y;
	dx = abs(x2 - x1); //we want the abs distance between x1 and x2
	dy = abs(y2 - y1);
	dy = dy * (-1);//we want the -abs distance between y1 and y2 (because y usually in the left side  
	if (x1 < x2) //we want to check the direction of the line 
	{
		sx = 1;
	}
	else
	{
		sx = -1;
	}
	if (y1 < y2) //we want to check the direction of the line 
	{
		sy = 1;
	}
	else
	{
		sy = -1;
	}
	err1 = dx + dy;
	while (true)
	{
		PutPixel(x1, y1, color);
		if (x1 == x2 && y1 == y2)
			break;
		err2 = 2 * err1;
		if (err2 >= dy)
		{
			x1 += sx;
			err1 += dy;
		}
		if (err2 <= dx)
		{
			y1 += sy;
			err1 += dx;
		}
	}
}

bool isPointLeftOfLine(const glm::vec2& p1, const glm::vec2& p2, const glm::ivec2& point)
{
	// find vector representing the line
	glm::vec2 v1 = p2 - p1;
	// rotate 90 degrees clock-wise
	glm::vec2 v2 = { v1.y, -v1.x };
	// find vector from p1 to the point
	glm::vec2 v3 = glm::vec2(point) - p1;
	// find sign of cos(angle) bettern v2 and v3
	float cross = glm::dot(v2, v3);
	// if angle is negative the point is on the left of the line
	return cross < 0;
}

// or in short (again we use CCW winding)
float edge_function(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& point)
{
	return (point.x - p2.x) * (p1.y - p2.y) - (point.y - p2.y) * (p1.x - p2.x);
}

bool pixelContainedIn2DTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::ivec2& pixel)
{
	// obj files stores triangles in counter-clockwise order by default
	bool inside = true;
	inside &= edge_function(p1, p2, pixel) >= 0;
	inside &= edge_function(p2, p3, pixel) >= 0;
	inside &= edge_function(p3, p1, pixel) >= 0;
	return inside;
}

glm::vec3 barycentric_coordinates(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::ivec2& pixel)
{
	// interpolate late the pixel on the given triangle and return the barycentric coordinates or weights of the pixel
	glm::vec2 p = pixel;
	// area of the triangle multiplied by 2
	float area = edge_function(v1, v2, v3);
	// signed area of the triangle v2v3p multiplied by 2 
	float w1 = edge_function(v2, v3, p);
	// signed area of the triangle v3v1p multiplied by 2 
	float w2 = edge_function(v3, v1, p);
	// signed area of the triangle v1v2p multiplied by 2 
	float w3 = edge_function(v1, v2, p);
	// barycentric coordinates are the areas of the sub-triangles divided by the area of the main triangle
	return glm::vec3{ w1, w2, w3 } / area;
}

float randomFloat()
{
	return static_cast<float>(rand()) / RAND_MAX;
}

void Renderer::DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& color)
{
	// rasterize the given triangle on the color buffer
	// p1, p2 and p3 are in raster space
	// convert the vertices of the current triangle to raster space
	glm::vec2 bbmin = { viewport_width_ - 1, viewport_height_ - 1 }, bbmax = { 0, 0 };
	const glm::vec2 points[3] = { p1, p2, p3 };
	for (int i = 0; i < 3; ++i) {
		if (points[i].x < bbmin.x) bbmin.x = points[i].x;
		if (points[i].y < bbmin.y) bbmin.y = points[i].y;
		if (points[i].x > bbmax.x) bbmax.x = points[i].x;
		if (points[i].y > bbmax.y) bbmax.y = points[i].y;
	}

	int xmin = std::max(0, std::min(viewport_width_ - 1, static_cast<int>(std::floor(bbmin.x))));
	int ymin = std::max(0, std::min(viewport_height_ - 1, static_cast<int>(std::floor(bbmin.y))));
	int xmax = std::max(0, std::min(viewport_width_ - 1, static_cast<int>(std::floor(bbmax.x))));
	int ymax = std::max(0, std::min(viewport_height_ - 1, static_cast<int>(std::floor(bbmax.y))));

	glm::vec3 random_color = glm::vec3(randomFloat(), randomFloat(), randomFloat());
	for (int y = ymin; y <= ymax; ++y)
	{
		for (int x = xmin; x <= xmax; ++x)
		{
			glm::ivec2 pixel = { x,y };
			if (pixelContainedIn2DTriangle(p1, p2, p3, pixel))
			{
				// calculate weights for current pixel
				glm::vec3 weights = barycentric_coordinates(p1, p2, p3, pixel);
				float depth = glm::dot(weights, { p1.z, p2.z, p3.z });
				if (DepthTest(x, y, depth))
				{
					PutDepth(x, y, depth);
					PutPixel(x, y, random_color);
				}
			}
		}
	}
}

glm::vec4 Renderer::CalculateFragColor(glm::vec3 pos, glm::vec3 norm, const Renderer::Uniform& u)
{

	//const glm::vec3 light_source = { 10.0f, 10.0f, 0.0f };
	//const float shininess = 200.0f;
	glm::vec4 FragColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (Light* light : u.lights)
	{
		glm::vec3 L = { 0.0f, 0.0f ,0.0f };
		switch (light->GetType())
		{
		case Light::LightType::Directional:
			L = -glm::normalize(light->GetLightSource());
			break;
		case Light::LightType::Positional:
			L = glm::normalize(light->GetLightSource() - pos);
			break;
		}
		const glm::vec3 E = glm::normalize(/* glm::vec3(0.0f, 0.0f, 0.0f) */ -pos); // we are in Eye Coordinates, so EyePos is (0,0,0)
		const glm::vec3 R = glm::normalize(-glm::reflect(L, norm));

		// calculate Ambient (light and model):
		glm::vec4 Iamb = glm::vec4(light->GetAmbient(), 1.0f);
		glm::vec4 IambModel = glm::vec4(u.material->GetAmbient(), 1.0f);

		// calculate Diffuse (light and model):
		glm::vec4 Idiff = glm::vec4(light->GetDiffuse(), 1.0f) * std::max(glm::dot(norm, L), 0.0f);
		glm::vec4 IdiffModel = glm::vec4(u.material->GetDiffuse(), 1.0f);

		// calculate Specular (light and model):
		glm::vec4 Ispec = glm::vec4(light->GetSpecular(), 1.0f) * std::powf(std::max(glm::dot(R, E), 0.0f), light->GetShininess());
		glm::vec4 IspecModel = glm::vec4(u.material->GetSpecular(), 1.0f);

		// write Total Color:
		glm::vec4 LightColor = Iamb * IambModel + Idiff * IdiffModel + Ispec * IspecModel;

		FragColor += LightColor;
	}

	return FragColor;
}

void Renderer::ShadeTriangle(const Renderer::Vertex& v1, const Renderer::Vertex& v2, const Renderer::Vertex& v3, const Uniform& u)
{
	// shade the given triangle resterizing and interpolating vertex attributes
	// vertex positions are in raster space
	glm::vec2 bbmin = { viewport_width_ - 1, viewport_height_ - 1 }, bbmax = { 0, 0 };
	const glm::vec2 points[3] = { v1.pos, v2.pos, v3.pos };
	for (int i = 0; i < 3; ++i) {
		if (points[i].x < bbmin.x) bbmin.x = points[i].x;
		if (points[i].y < bbmin.y) bbmin.y = points[i].y;
		if (points[i].x > bbmax.x) bbmax.x = points[i].x;
		if (points[i].y > bbmax.y) bbmax.y = points[i].y;
	}

	int xmin = std::max(0, std::min(viewport_width_ - 1, static_cast<int>(std::floor(bbmin.x))));
	int ymin = std::max(0, std::min(viewport_height_ - 1, static_cast<int>(std::floor(bbmin.y))));
	int xmax = std::max(0, std::min(viewport_width_ - 1, static_cast<int>(std::floor(bbmax.x))));
	int ymax = std::max(0, std::min(viewport_height_ - 1, static_cast<int>(std::floor(bbmax.y))));

	// Shade here:
	glm::vec4 colors[3] = {};
	glm::vec4 flatColor;
	if (u.mode == ShaderMode::Flat)
	{
		flatColor = CalculateFragColor(v2.v, v2.normal, u);
	}
	if (u.mode == ShaderMode::Gouraud)
	{
		colors[0] = CalculateFragColor(v1.v, v1.normal, u);
		colors[1] = CalculateFragColor(v2.v, v2.normal, u);
		colors[2] = CalculateFragColor(v3.v, v3.normal, u);
	}

	for (int y = ymin; y <= ymax; ++y)
	{
		for (int x = xmin; x <= xmax; ++x)
		{
			glm::ivec2 pixel = { x,y };
			if (pixelContainedIn2DTriangle(v1.pos, v2.pos, v3.pos, pixel))
			{
				// calculate weights for current pixel
				glm::vec3 weights = barycentric_coordinates(v1.pos, v2.pos, v3.pos, pixel);
				float depth = glm::dot(weights, { v1.pos.z, v2.pos.z, v2.pos.z });
				if (DepthTest(x, y, depth))
				{
					PutDepth(x, y, depth);

					glm::vec4 FragColor;
					if (u.mode == ShaderMode::Gouraud)
					{
						FragColor = weights.x * colors[0] + weights.y * colors[1] + weights.z * colors[2];
					}
					else if (u.mode == ShaderMode::Phong)
					{
						glm::vec3 pos = weights.x * v1.v + weights.y * v2.v + weights.z * v3.v;
						glm::vec3 norm = weights.x * v1.normal + weights.y * v2.normal + weights.z * v3.normal;
						FragColor = CalculateFragColor(pos, norm, u);
					}
					else if (u.mode == ShaderMode::Flat)
					{
						FragColor = flatColor;
					}

					if (GetFog())
					{
						float viewz = weights.x * v1.v.z + weights.y * v2.v.z + weights.z * v3.v.z;
						float np = 1.0f;
						float fr = 30.0f;
						float f = ((-viewz) - np) / (fr - np);
						glm::vec4 fog_color = { 0.5f, 0.5f, 0.5f, 1.0f };
						FragColor = f * fog_color + (1.0f - f) * FragColor;
					}

					PutPixel(x, y, FragColor);
				}
			}
		}
	}
}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenGLBuffer(); //Do not remove this line.
	color_buffer_ = new float[3 * w * h];
	depth_buffer_ = new float[w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
	ClearDepthBuffer(1.0f);
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex_);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc_);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc_);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)__(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,
		1, -1,
		-1,  1,
		-1,  1,
		1, -1,
		1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

}

void Renderer::CreateOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width_, viewport_height_, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width_, viewport_height_);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width_, viewport_height_, GL_RGB, GL_FLOAT, color_buffer_);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc_);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width_; i++)
	{
		for (int j = 0; j < viewport_height_; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::ClearDepthBuffer(float depth)
{
	for (int i = 0; i < viewport_width_; i++)
	{
		for (int j = 0; j < viewport_height_; j++)
		{
			PutDepth(i, j, depth);
		}
	}
}

void Renderer::Render(const Scene& scene)
{
	int half_width = viewport_width_ / 2;
	int half_height = viewport_height_ / 2;
	float ratio = float(viewport_width_) / float(viewport_height_);

	bool hide_others = scene.GetHideNotSelectedModels();
	bool hide_normals = scene.GetHideNormals();
	bool hide_box = scene.GetHideBox();

	Camera& cam = scene.GetActiveCamera();

	glm::mat4 view_matrix = cam.GetViewTransformation();

	glm::mat4 projection = cam.GetProjectionTransformation(ratio);
	glm::vec4 viewport(0.0f, 0.0f, float(viewport_width_), float(viewport_height_));

	int selected_model = scene.GetActiveModelIndex();
	int model_count = scene.GetModelCount();

	Uniform u = {};
	u.mode = shader_mode_;
	for (int i = 0; i < scene.GetLightCount(); ++i)
	{
		Light& light = scene.GetLight(i);
		u.lights.push_back(&light);
	}

	for (int i = 0; i < model_count; ++i)
	{
		if (hide_others && i != selected_model)
			continue;

		MeshModel& model = scene.GetModel(i);
		u.material = &model.GetMaterial();
		//u.ambient = scene.GetAmbient();
		//u.diffuse = scene.GetDiffuse();
		//u.specular = scene.GetSpecular();
		//u.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		//u.diffuse = glm::vec3(0.8f, 0.2f, 0.1f);
		//u.specular = glm::vec3(1.0f, 1.0f, 1.0f);

		int count = model.GetFacesCount();

		float normal_size = scene.GetNormalSize();

		glm::vec3 translation = model.GetTranslation();
		glm::vec3 rotation = model.GetRotation();
		glm::vec3 scale = model.GetScale();

		glm::mat4 rotate_x = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotate_y = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotate_z = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 rotation_matrix = rotate_x * rotate_y * rotate_z;

		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), translation) * rotation_matrix * glm::scale(glm::mat4(1.0f), scale);
		glm::mat4 model_view = view_matrix * model_matrix;

		// normals get special treatment
		glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_view));

		glm::vec3 v1 = model.GetVertex(0);
		glm::vec4 mv1 = model_matrix * glm::vec4(v1, 1.0f);
		v1 = mv1 / mv1.w;
		float bounding_box_x_min = v1.x;
		float bounding_box_x_max = v1.x;
		float bounding_box_y_min = v1.y;
		float bounding_box_y_max = v1.y;
		float bounding_box_z_min = v1.z;
		float bounding_box_z_max = v1.z;

		// we go through all the faces of the model to draw it
		for (int i = 0; i < count; i++)
		{
			// draw the face in wire frame
			const Face& face = model.GetFace(i);
			int ver1index = face.GetVertexIndex(0);
			int ver2index = face.GetVertexIndex(1);
			int ver3index = face.GetVertexIndex(2);

			// we set the color var to be red
			const glm::ivec3 color = glm::ivec3(1, 0, 0);

			// model space positions
			// CCW!!
			const glm::vec3& ver1 = model.GetVertex(ver1index - 1);
			const glm::vec3 & ver2 = model.GetVertex(ver2index - 1);
			const glm::vec3 & ver3 = model.GetVertex(ver3index - 1);
			const glm::vec3 & vercenter = (ver1 + ver2 + ver3) / 3.0f;

			glm::vec4 mver1 = model_matrix * glm::vec4(ver1, 1.0f);
			glm::vec4 mver2 = model_matrix * glm::vec4(ver2, 1.0f);
			glm::vec4 mver3 = model_matrix * glm::vec4(ver3, 1.0f);

			mver1 /= mver1.w;
			mver2 /= mver2.w;
			mver3 /= mver3.w;
			//glm::vec3 mvercenter = (mver1 + mver2 + mver3) / (3.0f);

			// world space positions
			std::array<glm::vec3, 3> verts = { glm::vec3(mver1), glm::vec3(mver2), glm::vec3(mver3) };
			for (const auto& v : verts)
			{
				// update bounding box min-max values
				bounding_box_x_min = std::min(bounding_box_x_min, v.x);
				bounding_box_x_max = std::max(bounding_box_x_max, v.x);
				bounding_box_y_min = std::min(bounding_box_y_min, v.y);
				bounding_box_y_max = std::max(bounding_box_y_max, v.y);
				bounding_box_z_min = std::min(bounding_box_z_min, v.z);
				bounding_box_z_max = std::max(bounding_box_z_max, v.z);
			}

			glm::vec3 pver1 = glm::project(ver1, view_matrix * model_matrix, projection, viewport);
			glm::vec3 pver2 = glm::project(ver2, view_matrix * model_matrix, projection, viewport);
			glm::vec3 pver3 = glm::project(ver3, view_matrix * model_matrix, projection, viewport);
			glm::vec3 pcenterver = glm::project(vercenter, view_matrix * model_matrix, projection, viewport);

			// Draw the faces
			//DrawLine(glm::ivec2(pver1.x, pver1.y), glm::ivec2(pver2.x, pver2.y), color);
			//DrawLine(glm::ivec2(pver2.x, pver2.y), glm::ivec2(pver3.x, pver3.y), color);
			//DrawLine(glm::ivec2(pver3.x, pver3.y), glm::ivec2(pver1.x, pver1.y), color);

			// Draw Triangle
			//srand((i * 444303127) % 0xFFFFFFFF);
			//DrawTriangle(pver1, pver2, pver3, color);

			// now for the normals
			int norm1index = face.GetNormalIndex(0);
			int norm2index = face.GetNormalIndex(1);
			int norm3index = face.GetNormalIndex(2);

			// we set the color var to be blue-ish
			const glm::vec3 norm_color = glm::vec3(0.2f, 0.2f, 0.8f);

			// model space normals
			glm::vec3 norm1 = model.GetNormal(norm1index - 1);
			glm::vec3 norm2 = model.GetNormal(norm2index - 1);
			glm::vec3 norm3 = model.GetNormal(norm3index - 1);
			glm::vec3 normcenter = (norm1 + norm2 + norm3) / (3.0f);

			glm::vec4 mnorm1 = normal_matrix * glm::vec4(norm1, 1.0f);
			glm::vec4 mnorm2 = normal_matrix * glm::vec4(norm2, 1.0f);
			glm::vec4 mnorm3 = normal_matrix * glm::vec4(norm3, 1.0f);
			glm::vec4 mcenternorm = normal_matrix * glm::vec4(normcenter, 1.0f);

			mnorm1 /= mnorm1.w;
			mnorm2 /= mnorm2.w;
			mnorm3 /= mnorm3.w;
			mcenternorm /= mcenternorm.w;

			// world space normals
			norm1 = glm::normalize(glm::vec3(mnorm1));
			norm2 = glm::normalize(glm::vec3(mnorm2));
			norm3 = glm::normalize(glm::vec3(mnorm3));
			normcenter = glm::normalize(glm::vec3(mcenternorm));

			// for drawing normals convert the model view verts to camera view verts
			glm::vec4 mvver1 = view_matrix * glm::vec4(verts[0], 1.0f);
			glm::vec4 mvver2 = view_matrix * glm::vec4(verts[1], 1.0f);
			glm::vec4 mvver3 = view_matrix * glm::vec4(verts[2], 1.0f);

			mvver1 /= mvver1.w;
			mvver2 /= mvver2.w;
			mvver3 /= mvver3.w;
			glm::vec3 mvercenter = (mvver1 + mvver2 + mvver3) / (3.0f);

			// use the world normals and texture coordinates and shade the triangle
			Vertex v1 = { mvver1, pver1, norm1,{ 0.0f, 0.0f } };
			Vertex v2 = { mvver2, pver2, norm2,{ 0.0f, 0.0f } };
			Vertex v3 = { mvver3, pver3, norm3,{ 0.0f, 0.0f } };

			ShadeTriangle(v1, v2, v3, u);
			//ShadePhongTriangle(v1, v2, v3, u);

			// project the points protruding along the normal from the verts and verts center 
			glm::vec3 pnorm1 = glm::project(glm::vec3(mvver1) + (norm1 * normal_size), glm::mat4(1.0f), projection, viewport);
			glm::vec3 pnorm2 = glm::project(glm::vec3(mvver2) + (norm2 * normal_size), glm::mat4(1.0f), projection, viewport);
			glm::vec3 pnorm3 = glm::project(glm::vec3(mvver3) + (norm3 * normal_size), glm::mat4(1.0f), projection, viewport);
			glm::vec3 pcenternorm = glm::project(mvercenter + (normcenter * normal_size), glm::mat4(1.0f), projection, viewport);

			// Draw the normals if the button "hide_noremal" is off
			if (!hide_normals)
			{
				DrawLine(glm::ivec2(pver1.x, pver1.y), glm::ivec2(pnorm1.x, pnorm1.y), norm_color);
				DrawLine(glm::ivec2(pver2.x, pver2.y), glm::ivec2(pnorm2.x, pnorm2.y), norm_color);
				DrawLine(glm::ivec2(pver3.x, pver3.y), glm::ivec2(pnorm3.x, pnorm3.y), norm_color);
				DrawLine(glm::ivec2(pcenterver.x, pcenterver.y), glm::ivec2(pcenternorm.x, pcenternorm.y), norm_color);
			}
		}

		// now draw the bounding box

		/*
		*
		*  8          7
		*    +------+
		*    |\      \
		*    | \    6 \
		*  5 +__+------+ 3
		*     \ |4     |
		*      \|      |
		*     1 +------+ 2
		*/

		const glm::vec3& bb_ver1 = { bounding_box_x_min, bounding_box_y_min, bounding_box_z_min };
		const glm::vec3& bb_ver2 = { bounding_box_x_max, bounding_box_y_min, bounding_box_z_min };
		const glm::vec3& bb_ver3 = { bounding_box_x_max, bounding_box_y_max, bounding_box_z_min };
		const glm::vec3& bb_ver4 = { bounding_box_x_min, bounding_box_y_max, bounding_box_z_min };
		const glm::vec3& bb_ver5 = { bounding_box_x_min, bounding_box_y_min, bounding_box_z_max };
		const glm::vec3& bb_ver6 = { bounding_box_x_max, bounding_box_y_min, bounding_box_z_max };
		const glm::vec3& bb_ver7 = { bounding_box_x_max, bounding_box_y_max, bounding_box_z_max };
		const glm::vec3& bb_ver8 = { bounding_box_x_min, bounding_box_y_max, bounding_box_z_max };

		const glm::vec3 bb_verts[] = {
			glm::project(bb_ver1, view_matrix, projection, viewport),
			glm::project(bb_ver2, view_matrix, projection, viewport),
			glm::project(bb_ver3, view_matrix, projection, viewport),
			glm::project(bb_ver4, view_matrix, projection, viewport),
			glm::project(bb_ver5, view_matrix, projection, viewport),
			glm::project(bb_ver6, view_matrix, projection, viewport),
			glm::project(bb_ver7, view_matrix, projection, viewport),
			glm::project(bb_ver8, view_matrix, projection, viewport),
		};

		int vertex_indicies[] = {
			1, 2, 2, 3,
			3, 4, 4, 1,
			1, 5, 2, 6,
			3, 7, 4, 8,
			5, 6, 6, 7,
			7, 8, 8, 5,
		};

		// Draw the bounding box if the button "hide bounding box" is off
		if (!hide_box)
		{
			for (int i = 0; i < (sizeof(vertex_indicies) / sizeof(vertex_indicies[0])); i += 2)
			{
				const glm::vec3& v1 = bb_verts[vertex_indicies[i] - 1];
				const glm::vec3& v2 = bb_verts[vertex_indicies[i + 1] - 1];
				DrawLine(glm::ivec2(v1.x, v1.y), glm::ivec2(v2.x, v2.y), glm::vec3(.2f, .8f, .2f));
			}
		}
		//function blur
		if (scene.GetBlur())
		{
			for (int i = 0; i < 1; ++i)
			{
				BlurFunction();
			}
		}

	}
}

bool Renderer::HasPixel(int i, int j)
{
	// decide if pixel i,j is valid
	if (i < 0) return false; if (i >= viewport_width_) return false;
	if (j < 0) return false; if (j >= viewport_height_) return false;
	return true;
}

void Renderer::FilterCreation()
{
	// intialising standard deviation to 1.0 
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization 
	double sum = 0.0;

	// generating NxN kernel 
	int half_width = gaussian_kern_width_ / 2;
	int half_height = gaussian_kern_height_ / 2;
	int xend = (gaussian_kern_width_ % 2 == 0) ? half_width : half_width + 1;
	int yend = (gaussian_kern_width_ % 2 == 0) ? half_width : half_width + 1;
	for (int x = -half_width; x < xend; x++) {
		for (int y = -half_height; y < yend; y++) {
			r = sqrt(x * x + y * y);
			float value = (exp(-(r * r) / s)) / (M_PI * s);
			gaussian_kern_[G_INDEX(gaussian_kern_width_, x + half_width, y + half_height)] = value;
			sum += value;
		}
	}

	// normalising the Kernel 
	for (int i = 0; i < gaussian_kern_width_; ++i)
		for (int j = 0; j < gaussian_kern_height_; ++j)
			gaussian_kern_[G_INDEX(gaussian_kern_width_, i, j)] /= sum;
}

void Renderer::BlurFunction()
{
	std::cout << "hello 0" << std::endl;
	// first blur and save result to a temporary buffer
	float* gaussian_buffer_ = new float[3 * viewport_width_ * viewport_height_];
	// filter using gaussian kernel
	int half_width = gaussian_kern_width_ / 2;
	int half_height = gaussian_kern_height_ / 2;
	int xend = (gaussian_kern_width_ % 2 == 0) ? half_width : half_width + 1;
	int yend = (gaussian_kern_width_ % 2 == 0) ? half_width : half_width + 1;
	for (int i = 0; i < viewport_width_; ++i)
	{
		for (int j = 0; j < viewport_height_; ++j)
		{
			glm::vec3 filtered_color = { 0.0f, 0.0f, 0.0f };
			for (int x = -half_width; x < xend; x++) {
				for (int y = -half_height; y < yend; y++) {
					if (!HasPixel(i + x, j + y))
						continue;
					float w = gaussian_kern_[G_INDEX(gaussian_kern_width_, x + half_width, y + half_height)];
					filtered_color += w * GetPixel(i + x, j + y);
				}
			}
			gaussian_buffer_[INDEX(viewport_width_, i, j, 0)] = filtered_color.x;
			gaussian_buffer_[INDEX(viewport_width_, i, j, 1)] = filtered_color.y;
			gaussian_buffer_[INDEX(viewport_width_, i, j, 2)] = filtered_color.z;
		}
	}
	// next copy the temporary buffer back t othe color buffer
	memcpy(color_buffer_, gaussian_buffer_, 3 * viewport_width_ * viewport_height_ * sizeof(float));
	delete[] gaussian_buffer_;
}

int Renderer::GetViewportWidth() const
{
	return viewport_width_;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height_;
}

void Renderer::SetFog(bool fog)
{
	fog_ = fog;
}

bool Renderer::GetFog() const
{
	return fog_;
}