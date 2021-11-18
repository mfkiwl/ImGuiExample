#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ImGui/imgui.h>
#include <ImGui/impl/imgui_impl_glfw.h>
#include <ImGui/impl/imgui_impl_opengl3.h>

#include "../include/gl_camera.h"
#include "../include/gl_shader.h"
#include "../include/utility.hpp"
#include "../include/model.h"
#include "../include/builtin_obj.h"
#include "../include/window_manager.h"
#include "../include/skybox_manager.h"
#include "../include/framebuffers_manager.h"
#include "../include/uniform_buffer_manager.h"
#include "../shaders/post_process/post_process_shaders.hpp"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "assimp-vc143-mt.lib")

//cast char8_t to char
#if _MSVC_LANG >= 202002L	/*CXX20*/
#define CAST_U8(t) reinterpret_cast<const char*>(u8##t)	
#else	
#define CAST_U8(t) u8##t
#endif

//setting
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
const std::string EXAMPLE_NAME = "ImGui Example 2: Framebuffer";
const unsigned int BINDING_POINT_0 = 0;
const char* GLSL_VER = "#version 330";

//camera
GLCamera camera(glm::vec3(0.0f, 0.0f, 10.0f));

//skybox
std::vector<std::string> faces = {
	"res/textures/skybox/right.jpg",
	"res/textures/skybox/left.jpg",
	"res/textures/skybox/top.jpg",
	"res/textures/skybox/bottom.jpg",
	"res/textures/skybox/front.jpg",
	"res/textures/skybox/back.jpg"
};

int main()
{
	//create window
	WindowManager windowMgr(camera, SCR_WIDTH, SCR_HEIGHT, EXAMPLE_NAME, glfwGetPrimaryMonitor(), nullptr);
	windowMgr.SetCallback();

	//create framebuffers
	//----------------
	FramebuffersManager DefaultSpace(windowMgr.window);
	DefaultSpace.CreateScreenQuad(1);
	FramebuffersManager PostProcSpace(windowMgr.window);
	PostProcSpace.CreateScreenQuad(1);

	//configure global OpenGL state
	//-----------------------------
	glEnable(GL_DEPTH_TEST);

	//shaders
	//-------
	std::map<std::string, GLShader> shaders = {
		{"red",		GLShader("shaders/default.vert", "shaders/red.frag")	},
		{"green",	GLShader("shaders/default.vert", "shaders/green.frag")	},
		{"blue",	GLShader("shaders/default.vert", "shaders/blue.frag")	},
		{"yellow",	GLShader("shaders/default.vert", "shaders/yellow.frag")	}
	};
	GLShader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

	GLShader screenShaderDefault(PostProcessShaders::DefaultVert, PostProcessShaders::DefaultFrag);
	GLShader screenShaderInversion(PostProcessShaders::DefaultVert, PostProcessShaders::Inversion);
	GLShader screenShaderGrayscale(PostProcessShaders::DefaultVert, PostProcessShaders::Grayscale);
	GLShader* postProcPtr = &screenShaderDefault;

	//shader config
	//-------------
	for (auto& i : shaders)
	{
		UniformBlockBind(i.second, "Matrices", BINDING_POINT_0);
	}
	skyboxShader.Use()
		.SetInt("skyboxTex", 0);
	screenShaderDefault.Use()
		.SetInt("screenTex", 0);
	screenShaderInversion.Use()
		.SetInt("screenTex", 0);
	screenShaderGrayscale.Use()
		.SetInt("screenTex", 0);

	//create UBO
	//----------
	UniformBufferManager UBO(2 * sizeof(glm::mat4));
	UBO.BindPoint(BINDING_POINT_0);
	auto [_width, _height] = GetFramebufferSize(windowMgr.window);
	auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(_width) / static_cast<float>(_height), 0.1f, 100.0f);
	UBO.SetSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
	
	//create scene objects
	//--------------------
	BuiltInObject cube(BIOType::OBJ_CUBE);

	//create skybox
	//-------------
	SkyboxManager skyboxMgr(faces);

	//set imgui
	//---------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImFont* font1 = io.Fonts->AddFontFromFileTTF(CAST_U8("res\\fonts\\微软雅黑.ttc"), 18.0f, NULL,
												 io.Fonts->GetGlyphRangesChineseFull());

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(windowMgr.window, true);
	ImGui_ImplOpenGL3_Init(GLSL_VER);
	//draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	windowMgr.Show(300, 100);

	//render loop
	//-----------
	while (!glfwWindowShouldClose(windowMgr.window))
	{
		windowMgr.ProcessInput();

		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//render
		//------
		//bind to framebuffer
		//-----------------------------------------------------------------------------------------------------------------
		DefaultSpace.Bind();
		glEnable(GL_DEPTH_TEST);

		//make sure we clear the framebuffer's content
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set MVP Matrix
		auto model = glm::mat4(1.0f);
		auto view = camera.GetViewMatrix();
		UBO.SetSubData(0, sizeof(glm::mat4), glm::value_ptr(view));

		//draw four cubes
		//red
		shaders.at("red").Use();
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));	//move top-left
		cube.BuiltInObjRender(shaders.at("red"), model, true);
		//green
		shaders.at("green").Use();
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));	//move top-left
		cube.BuiltInObjRender(shaders.at("green"), model, true);
		//blue
		shaders.at("blue").Use();
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));	//move top-left
		cube.BuiltInObjRender(shaders.at("blue"), model, true);
		//yellow
		shaders.at("yellow").Use();
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));	//move top-left
		cube.BuiltInObjRender(shaders.at("yellow"), model, true);

		//skybox render
		glDepthFunc(GL_LEQUAL);
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxMgr.Rneder(skyboxShader, view, projection);
		glDepthFunc(GL_LESS);

		//bind back to default framebuffer
		//-----------------------------------------------------------------------------------------------------------------
		DefaultSpace.UnBind();
		glDisable(GL_DEPTH_TEST);
		//clear all relevant buffers
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		PostProcSpace.Bind();
		auto transform = glm::mat4(1.0f);
		DefaultSpace.Render(*postProcPtr);
		PostProcSpace.UnBind();

		static auto SceneWindowSize = ImVec2(600, 400);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(SceneWindowSize, ImGuiCond_Always);
		ImGui::Begin(CAST_U8("渲染窗口"));
		//auto pos = ImGui::GetCursorScreenPos();
		auto pos = ImVec2(0, 0);
		auto drawList = ImGui::GetWindowDrawList();
		drawList->AddImage((void*)PostProcSpace.GetTexColorBuffer(),
						   pos,
						   ImVec2(pos.x+ 600, pos.y+400),
						   ImVec2(0, 1),
						   ImVec2(1, 0));
		SceneWindowSize = ImVec2(windowMgr.GetScrWidth()/2, windowMgr.GetScrHeight()/2);
		
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(windowMgr.GetScrWidth() - 300, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
		ImGui::Begin("Post Process", (bool*)1);
		static int postProcIdx = 0;
		if (ImGui::Combo("Post Process", &postProcIdx, "Default\0Inversion\0Grayscale\0"))
		{
			switch (postProcIdx)
			{
			case 0:
				postProcPtr = &screenShaderDefault;
				break;
			case 1:
				postProcPtr = &screenShaderInversion;
				break;
			case 2:
				postProcPtr = &screenShaderGrayscale;
				break;
			}
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		windowMgr.UpData();
	}

	cube.Delete();
	DefaultSpace.Delete();
	PostProcSpace.Delete();
	skyboxMgr.Delete();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}