#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <imgui/imgui.h>
#include <stdio.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <fmt/format.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"
#include "Camera.h"


/**
* Fields
*/
bool show_demo_window = false;
bool show_another_window = false;
glm::vec4 clear_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);


/**
* Function declarations
*/
static void GlfwErrorCallback(int error, const char* description);
bool AddModelHelper(Scene& scene, const std::string& modelPath);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer, int& modelIndex);
void HandleInput(ImGuiIO& io, Scene& scene);

/**
* Function implementation
*/
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}


int main(int argc, char **argv)
{
	int windowWidth = 1980, windowHeight = 1080;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();

	if (!AddModelHelper(scene, "..\\Data\\sphere.obj")) return -1;
	if (!AddModelHelper(scene, "..\\Data\\demo.obj")) return -1;
	if (!AddModelHelper(scene, "..\\Data\\bunny.obj")) return -1;
	if (!AddModelHelper(scene, "..\\Data\\camera.obj")) return -1;
	if (!AddModelHelper(scene, "..\\Data\\banana.obj")) return -1;
	if (!AddModelHelper(scene, "..\\Data\\pawn.obj")) return -1;

	int modelIndex = 0;

	std::shared_ptr<Camera> cam = std::make_shared<Camera>();
	cam->SetCameraLookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scene.AddCamera(cam);

	std::shared_ptr<Light> light = std::make_shared<Light>();
	light->GetType(Light::LightType::Positional);
	scene.AddLight(light);

	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene, renderer, modelIndex);
		HandleInput(io, scene);
		RenderFrame(window, scene, renderer, io);
	}

	Cleanup(window);
	return 0;
}

bool AddModelHelper(Scene& scene, const std::string& modelPath)
{
	// make sure to use a relative path so project is not machine specific
	std::shared_ptr<MeshModel> m = Utils::LoadMeshModel(modelPath);

	if (!m) {
		std::cout << "could not load model!" << std::endl;
		return false;
	}

	scene.AddModel(m);
	return true;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.ClearDepthBuffer(1.0f);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer, int& modelIndex)
{
	/**
	* MeshViewer menu
	*/
	ImGui::Begin("MeshViewer Menu");

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					if (AddModelHelper(scene, outPath)) {
						// replace the shown model in another one we load
						modelIndex++;
						scene.SetActiveModelIndex(modelIndex);
					}
					free(outPath);
				}

			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);

	ImGui::End();

	static float f = 0.0f;
	static int size = 120;
	static glm::vec3 translate = { 0.0f, 0.0f, 0.0f };
	static glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	static glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	static int scale_uniformly = 1;
	static float normal_size = 1.0f;
	static int blur = 0;
	static int fog = 1;

	// we create the GUI window for the model transformation
	ImGui::Begin("Model Transformation Window");


	ImGui::Text("Model Translation:");
	ImGui::SliderFloat("Translate X: ", &translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat("Translate Y: ", &translate.y, -10.0f, 10.0f);
	ImGui::SliderFloat("Translate Z: ", &translate.z, -10.0f, 10.0f);
	ImGui::Text("Model Rotation:");
	ImGui::SliderFloat("Rotation X: ", &rotation.x, -360.0f, 360.0f);
	ImGui::SliderFloat("Rotation Y: ", &rotation.y, -360.0f, 360.0f);
	ImGui::SliderFloat("Rotation Z: ", &rotation.z, -360.0f, 360.0f);
	ImGui::Text("Model Scale:");
	if (ImGui::SliderFloat("Scale X: ", &scale.x, 0.1f, 20.0f) && scale_uniformly)
		scale.y = scale.z = scale.x;
	if (ImGui::SliderFloat("Scale Y: ", &scale.y, 0.1f, 20.0f) && scale_uniformly)
		scale.x = scale.z = scale.y;
	if (ImGui::SliderFloat("Scale Z: ", &scale.z, 0.1f, 20.0f) && scale_uniformly)
		scale.x = scale.y = scale.z;


	ImGui::Text("Uniform Scale:");
	ImGui::RadioButton("Non-Uniform", &scale_uniformly, 0); ImGui::SameLine();
	ImGui::RadioButton("Uniform", &scale_uniformly, 1);
	static glm::vec3 model_ambient_color = glm::vec3(0.0f, 0.0f, 0.0f);
	static glm::vec3 model_diffuse_color = glm::vec3(149.0f / 255.0f, 66.0f / 255.0f, 66.0f / 255.0f);
	static glm::vec3 model_specular_color = glm::vec3(1.0f, 1.0f, 1.0f);


	ImGui::ColorEdit3("Model Ambient Color", (float*)& model_ambient_color);
	ImGui::ColorEdit3("Model Diffuse Color", (float*)& model_diffuse_color);
	ImGui::ColorEdit3("Model Specular Color", (float*)& model_specular_color);

	static float model_shininess = 200.0f;
	ImGui::SliderFloat("Model Shininess ", &model_shininess, 1.0f, 200.0f);
	ImGui::Text("Normals:");
	ImGui::SliderFloat("Normals Size: ", &normal_size, 0.1f, 10.0f);
	static int render_mode = 1;
	ImGui::Text("Shader Mode:");
	ImGui::RadioButton("Gouraud", &render_mode, 0); ImGui::SameLine();
	ImGui::RadioButton("Phong", &render_mode, 1); ImGui::SameLine();
	ImGui::RadioButton("Flat", &render_mode, 2);
	ImGui::RadioButton("Blur off", &blur, 0);
	ImGui::RadioButton("Blur on", &blur, 1);
	ImGui::RadioButton("Fog off", &fog, 0);
	ImGui::RadioButton("Fog on", &fog, 1);

	renderer.SetRenderMode(static_cast<Renderer::ShaderMode>(render_mode));
	scene.SetBlur(blur == 1);
	scene.SetNormalSize(normal_size);
	renderer.SetFog(fog == 1);

	std::vector<const char*> items;
	for (int i = 0; i < scene.GetModelCount(); i++)
	{
		const std::string& name = scene.GetModel(i).GetModelName();
		items.push_back(name.c_str());
	}

	//when the user clicks the listbox this function returns true
	if (ImGui::ListBox("select model:", &modelIndex, &items[0], items.size(), 4)) {
		scene.SetActiveModelIndex(modelIndex);
		// reset sliders:
		MeshModel& model = scene.GetActiveModel();
		translate = model.GetTranslation();
		rotation = model.GetRotation();
		scale = model.GetScale();
		model_shininess = model.GetMaterial().GetShininess();
		model_ambient_color = model.GetMaterial().GetAmbient();
		model_diffuse_color = model.GetMaterial().GetDiffuse();
		model_specular_color = model.GetMaterial().GetSpecular();
	}
	//else, didn't click the listbox but did click somthing else
	else {
		// update model transforms
		MeshModel& model = scene.GetActiveModel();
		model.SetTranslation(translate);
		model.SetRotation(rotation);
		model.SetScale(scale);
		model.GetMaterial().SetShininess(model_shininess);
		model.GetMaterial().SetAmbient(model_ambient_color);
		model.GetMaterial().SetDiffuse(model_diffuse_color);
		model.GetMaterial().SetSpecular(model_specular_color);
	}

	static bool hide_not_selected = true;
	static bool hide_normals = true;
	static bool hide_box = true;

	ImGui::Checkbox("Hide not selected", &hide_not_selected);
	scene.SetHideNotSelectedModels(hide_not_selected);

	ImGui::Checkbox("Hide normals", &hide_normals);
	scene.SetHideNormals(hide_normals);

	ImGui::Checkbox("Hide bounding box", &hide_box);
	scene.SetHideBox(hide_box);

	//Reset of the model - go back to the uploaded size and shape
	if (ImGui::Button("Reset"))
	{
		f = 0.0f;
		size = 120;
		translate = { 0.0f, 0.0f, 0.0f };
		rotation = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
		scale_uniformly = 1;
		normal_size = 1.0f;
	}

	ImGui::End();

	static float camera_pan = 0.0f;
	static float camera_dolly = 0.0f;
	static float camera_spin = 0.0f;
	static float camera_orbit = 0.0f;
	static float camera_zoom = 60.0f;

	ImGui::Begin("Camera Controls");

	bool changed = false;
	changed |= ImGui::SliderFloat("Camera Pan: ", &camera_pan, -10.0f, 10.0f);
	changed |= ImGui::SliderFloat("Camera Dolly: ", &camera_dolly, -10.0f, 10.0f);
	changed |= ImGui::SliderFloat("Camera Spin: ", &camera_spin, -360.0f, 360.0f);
	changed |= ImGui::SliderFloat("Camera Orbit: ", &camera_orbit, -360.0f, 360.0f);
	changed |= ImGui::SliderFloat("Camera Zoom: ", &camera_zoom, 10.0f, 180.0f);

	std::vector<const char*> camera_names;
	for (int i = 0; i < scene.GetCameraCount(); i++)
	{
		const std::string& name = scene.GetCamera(i).GetName();
		camera_names.push_back(name.c_str());
	}

	static int current_camera = 0;
	if (ImGui::ListBox("select camera:", &current_camera, &camera_names[0], camera_names.size(), 4)) {
		scene.SetActiveCameraIndex(current_camera);
		// reset sliders:
		Camera& cam = scene.GetActiveCamera();
		camera_pan = cam.GetPan();
		camera_dolly = cam.GetDolly();
		camera_spin = cam.GetSpin();
		camera_orbit = cam.GetOrbit();
		camera_zoom = cam.GetZoom();

	}
	else {
		// update camera controls
		Camera& camera = scene.GetActiveCamera();
		camera.SetPan(camera_pan);
		camera.SetSpin(camera_spin);
		camera.SetOrbit(camera_orbit);
		camera.SetDolly(camera_dolly);
		camera.SetZoom(camera_zoom);
	}

	if (ImGui::Button("Add camera"))
	{

		std::shared_ptr<Camera> cam = std::make_shared<Camera>();
		cam->SetCameraLookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		scene.AddCamera(cam);
	}

	ImGui::End();

	ImGui::Begin("Light Controls");

	static glm::vec3 ambient_color = glm::vec3(0.0f, 0.0f, 0.0f);
	static glm::vec3 diffuse_color = glm::vec3(149.0f / 255.0f, 66.0f / 255.0f, 66.0f / 255.0f);
	static glm::vec3 specular_color = glm::vec3(1.0f, 1.0f, 1.0f);

	changed = false;

	static int light_type = (int)Light::LightType::Directional;
	ImGui::Text("Light type:");
	changed |= ImGui::RadioButton("Directional", &light_type, (int)Light::LightType::Directional); ImGui::SameLine();
	changed |= ImGui::RadioButton("Positional", &light_type, (int)Light::LightType::Positional);

	changed |= ImGui::ColorEdit3("Ambient Color", (float*)& ambient_color);
	changed |= ImGui::ColorEdit3("Diffuse Color", (float*)& diffuse_color);
	changed |= ImGui::ColorEdit3("Specular Color", (float*)& specular_color);

	static float shininess = 200.0f;
	ImGui::SliderFloat("shininess ", &shininess, 1.0f, 200.0f);

	static glm::vec3 light_source = { 10.0f, 10.0f, 0.0f };
	ImGui::Text("Light Source:");
	ImGui::SliderFloat("X: ", &light_source.x, -10.0f, 10.0f);
	ImGui::SliderFloat("Y: ", &light_source.y, -10.0f, 10.0f);
	ImGui::SliderFloat("Z: ", &light_source.z, -10.0f, 10.0f);

	std::vector<const char*> light_names;
	for (int i = 0; i < scene.GetLightCount(); i++)
	{
		const std::string& name = scene.GetLight(i).GetName();
		light_names.push_back(name.c_str());
	}

	static int current_light = 0;
	if (ImGui::ListBox("select light:", &current_light, &light_names[0], light_names.size(), 4)) {
		scene.SetActiveLightIndex(current_light);
		// reset sliders:
		Light& light = scene.GetActiveLight();
		light_type = (int)light.GetType();
		light_source = light.GetLightSource();
		shininess = light.GetShininess();
		ambient_color = light.GetAmbient();
		diffuse_color = light.GetDiffuse();
		specular_color = light.GetSpecular();

	}
	else {
		// update light controls
		Light& light = scene.GetActiveLight();
		light.GetType((Light::LightType)light_type);
		light.SetLightSource(light_source);
		light.SetShininess(shininess);
		light.SetAmbient(ambient_color);
		light.SetDiffuse(diffuse_color);
		light.SetSpecular(specular_color);
	}

	if (ImGui::Button("Add light"))
	{
		std::shared_ptr<Light> light = std::make_shared<Light>();
		scene.AddLight(light);
	}

	ImGui::End();

}

void HandleInput(ImGuiIO& io, Scene& scene)
{
	if (!io.WantCaptureKeyboard)
	{
		// TODO: Handle keyboard events here
		if (io.KeysDown[65])
		{
			// A key is down
			// Use the ASCII table for more key codes (https://www.asciitable.com/)
		}
	}

	static glm::ivec2 right_click_pos;
	static glm::mat4 click_translation;
	static glm::mat4 click_rotation;
	Camera& cam = scene.GetActiveCamera();

	if (!io.WantCaptureMouse)
	{
		if (io.MouseClicked[0])
		{
			// Left mouse Click
			fmt::print("Right Click {} {}\n", io.MousePos.x, io.MousePos.y);
			right_click_pos = { io.MousePos.x, io.MousePos.y };
		}
		else if (io.MouseDown[0])
		{
			// Left mouse Drag
			fmt::print("Right Drag {} {}\n", io.MousePos.x, io.MousePos.y);
			glm::ivec2 drag_click_pos = { io.MousePos.x, io.MousePos.y };
			glm::ivec2 drag = drag_click_pos - right_click_pos;
			fmt::print("           {} {}\n", drag.x, drag.y);
		}
		else if (io.MouseReleased[0])
		{
			// Left mouse Release
			fmt::print("Right Release\n");
		}

	}
}