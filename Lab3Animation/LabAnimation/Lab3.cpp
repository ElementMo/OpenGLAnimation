#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL/freeglut.h>
#include "Utils.h"
#include "Model.h"
#include "Scene.h"
#include "Lab3.h"

// ----------- Program Parameters -----
static int window_width = 1280;
static int window_height = 720;
static float frameRate = 60.0f;
int viewport_width = window_width;
int viewport_height = window_height;

int frameCount = 0;
float deltaTime = 0;

int mouseX, mouseY;
int pmouseX, pmouseY;
bool is_hovering = false;

// ----------- Camera Parameters -------
float rotationX, rotationY;
float fov = 75;
float cam2pref = 20;
float cam2prefLerp = cam2pref;
bool isOrtho = false;
float cam_pos[3] = { 0, 0, 0 };

// ----------- Scene Parameters ---------
float m_trans[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
bool my_tool_active = true;



// ----------- Scene Parameters ---------
GLfloat light_position[] = { -10.0, 10.0, -10.0, 1.0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };

Model box("box.obj");
Model cylinder("cylinder.obj");
Model icoSphere("icoSphere.obj");
Model box_floor("floor.obj");
float ground_col[3] = { 0.8f, 0.6f, 0.9f };
Model ground("ground", ground_col);
Model light_bulb("LightBulb.obj");

// ----------- Physics Scene -------------
Scene scene(deltaTime, 1);

float param1 = 0;
float param2 = 0;
float param3 = 0;
float param4 = 0;
float param_offset_1[3] = { 3, 4, 0 };
float param_offset_2[3] = { -3, 3, 0 };

int rigidBodyModel = 0;

void setup() {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);

	rotationX = 4;
	rotationY = 10.67;

	glEnable(GL_LIGHT0);

	light_bulb.setRenderMode(GL_FILL);

	box.setRenderMode(GL_FILL);
	cylinder.setRenderMode(GL_FILL);
	icoSphere.setRenderMode(GL_FILL);
	box_floor.setRenderMode(GL_FILL);

	scene.Reset(box, box_floor);
}


void draw(glm::mat4 m_vp) {
	//std::cout << rotationX << "--" << rotationY << std::endl;
	// Draw Ground
	ground.render(m_vp, glm::mat4(1.0), false);

	light_bulb.render(m_vp, glm::translate(glm::mat4(1.0f), glm::vec3(light_position[0], light_position[1], light_position[2])), false);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Setup Physics Scene
	scene.dt = deltaTime;

	// Update Scene Lighting
	scene.SetLighting(light_position, light_ambient, light_diffuse, light_specular);

	// Calculate Collision and Apply Force
	scene.Step();

	// Redner rigid bodies in scene
	scene.Render(m_vp);
}

void imgui_func() {
	ImGui::Begin("Inspector", &my_tool_active, ImGuiWindowFlags_MenuBar);

	ImGui::SliderFloat("Cam FOV", &fov, 10.0f, 170.0f);
	ImGui::Checkbox("IsOrtho", &isOrtho);

	//ImGui::DragFloat3("Param1", param_offset_1, 0.1f, -10.0f, 10.0f);
	//ImGui::DragFloat3("Param2", param_offset_2, 0.1f, -10.0f, 10.0f);


	ImGui::DragFloat3("Light Pos", light_position, -10.0f, 10.0f, -10.0f);
	//ImGui::ColorPicker4("Light Ambient", light_ambient);
	//ImGui::ColorPicker4("Light Diffuse", light_diffuse);
	//ImGui::ColorPicker4("Light Specular", light_specular);

	ImGui::Combo("RigidBody Model", &rigidBodyModel, "Box\0Cylinder\0IcoSphere");
	if (ImGui::Button("Reset")) {
		scene.Clear();
		if (rigidBodyModel == 0)
			scene.Reset(box, box_floor);
		if (rigidBodyModel == 1)
			scene.Reset(cylinder, box_floor);
		if (rigidBodyModel == 2)
			scene.Reset(icoSphere, box_floor);
	}

	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 350));

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Path", "Ctrl+S")) {
				// TODO: Do something!!!!!!!
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	is_hovering = ImGui::IsAnyWindowFocused() || ImGui::IsAnyItemFocused();
	ImGui::End();

}

void glut_display_func() {
	// Setup & Render ImGui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	imgui_func();
	ImGui::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ---------- Mouse Control -----------------
	cam2pref = lerp(cam2pref, cam2prefLerp, 0.2f);
	rotationX += (mouseX - pmouseX) * 0.005f;
	rotationY += (mouseY - pmouseY) * 0.01f;

	// ---------- Camera Control -----------------
	cam_pos[0] = cos(rotationX - PI / 2) * cam2pref;
	cam_pos[1] = rotationY;
	cam_pos[2] = sin(rotationX - PI / 2) * cam2pref;

	// ---------- View & Projection Matrices -----------
	glm::mat4 m_persp = glm::perspective(glm::radians(fov), viewport_width / (float)viewport_height, 0.1f, 100.0f);
	float w = map(glm::tan(glm::radians(fov / 2)), 0, PI / 2, 2, 8);
	float h = w / (float)viewport_width * viewport_height;
	glm::mat4 m_ortho = glm::ortho(-w, w, -h, h, 0.1f, 100.0f);
	glm::mat4 m_proj = isOrtho ? m_ortho : m_persp;
	glm::mat4 m_view = glm::lookAt(glm::make_vec3(cam_pos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_vp = m_proj * m_view;

	draw(m_vp);

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	glutSwapBuffers();
	glFlush();

	pmouseX = mouseX;
	pmouseY = mouseY;

	frameCount++;
}

void glut_reshape_func(int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	viewport_width = width;
	viewport_height = height;
	// ImGui callback
	ImGui_ImplGLUT_ReshapeFunc(width, height);
	std::cout << "reshape" << std::endl;
}

void glut_timer_func(int val) {
	glut_display_func();
	deltaTime = 1.0f / frameRate;
	glutTimerFunc(1000.0 / frameRate, glut_timer_func, 0);
}

void glut_motion_func(int x, int y) {
	if (!is_hovering) {
		mouseX = x;
		mouseY = y;
	}
	// ImGui callback
	ImGui_ImplGLUT_MotionFunc(x, y);
}

void glut_mouse_func(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		mouseX = x;
		mouseY = y;
		pmouseX = x;
		pmouseY = y;
	}
	if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) return;
		cam2prefLerp = (button == 3) ? cam2prefLerp -= 0.5f : cam2prefLerp += 0.5f;
	}
	// ImGui callback
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void glut_keyboard_func(unsigned char key, int x, int y) {
	std::cout << key << std::endl;
	// ImGui callback
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}


int main(int argc, char** argv) {
	// Setup Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Lab1");

	// Lab Program Setup
	setup();

	// Call Back Functions
	glutReshapeFunc(glut_reshape_func);
	glutTimerFunc(1000.0 / frameRate, glut_timer_func, 0);
	glutMotionFunc(glut_motion_func);
	glutMouseFunc(glut_mouse_func);
	glutKeyboardFunc(glut_keyboard_func);
	glutDisplayFunc(glut_display_func);

	// ImGui Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	ImGui_ImplGLUT_Init();
	//ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	// Setup Graphics
	glClearColor(0.05f, 0.05f, 0.05f, 0.0f);
	glutMainLoop();

	// ImGui Destroy
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}