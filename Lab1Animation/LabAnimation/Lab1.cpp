#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Lab1.h"
#include "Utils.h"
#include "Model.h"

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
float fov = 60;
float cam2pref = 5;
float cam2prefLerp = cam2pref;
bool isOrtho = false;
float cam_pos[3] = { 0, 0, 0 };

// ----------- Temp Parameters ---------
float m_trans[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// ----------- Scene Parameters ---------
float teapot_col[3] = { 0.8f, 0.4f, 0.5f };
Model teapot("teapot", teapot_col);
float teacup_col[3] = { 0.4f, 0.6f, 0.9f };
Model teacup("teacup", teacup_col);
float ground_col[3] = { 0.8f, 0.6f, 0.9f };
Model ground("ground", ground_col);

void setup(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);

	rotationX = 0.5;
	rotationY = 2;

	Quaternion q1(0.5f, 0.86f, 0.0f, 0.0f);
	Quaternion q2 = q1.conj();
	std::cout << q2.w << q2.x << q2.y << q2.z << std::endl;
}

void draw(glm::mat4 m_vp) {
	// Draw Ground
	ground.render(glm::value_ptr(m_vp));
	// Draw Models
	glm::mat4 m_model_1 = glm::make_mat4(m_trans);
	teapot.render(glm::value_ptr(m_vp));
	teacup.render(glm::value_ptr(m_vp * m_model_1));
}

void imgui_func() {

	ImGui::SliderFloat("Cam FOV", &fov, 10.0f, 170.0f);
	ImGui::Checkbox("IsOrtho", &isOrtho);
	ImGui::SliderFloat("Trans X", &m_trans[12], -2.0f, 2.0f);
	ImGui::SliderFloat("Trans Y", &m_trans[13], -2.0f, 2.0f);
	ImGui::SliderFloat("Trans Z", &m_trans[14], -2.0f, 2.0f);

	ImGui::SetWindowPos(ImVec2(0, 0));

	is_hovering = ImGui::IsWindowFocused() || ImGui::IsItemFocused();
}

void glut_display_func() {
	// Setup & Render ImGui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	imgui_func();
	ImGui::Render();
	
	glClear(GL_COLOR_BUFFER_BIT);

	// ---------- Mouse Control -----------------
	cam2pref = lerp(cam2pref, cam2prefLerp, 0.2f);
	rotationX += (mouseX - pmouseX) * 0.005f;
	rotationY += (mouseY - pmouseY) * 0.01f;

	// ---------- Camera Control -----------------
	cam_pos[0] = cos(rotationX - PI / 2) * cam2pref;
	cam_pos[1] = rotationY;
	cam_pos[2] = sin(rotationX - PI / 2) * cam2pref;

	glm::mat4 m_persp = glm::perspective(glm::radians(fov), viewport_width / (float)viewport_height, 0.1f, 100.0f);
	float w = map(glm::tan(glm::radians(fov / 2)), 0, PI/2, 2, 8);
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
	deltaTime += 1000.0 / frameRate;
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
	glViewport(0, 0, window_width, window_height);
	gluPerspective(90, window_width / window_height, 0, 0000);
	glutCreateWindow("Lab1");


	// Lab Program Setup
	setup();

	// Call Back Functions
	glutDisplayFunc(glut_display_func);
	glutReshapeFunc(glut_reshape_func);
	glutTimerFunc(1000.0 / frameRate, glut_timer_func, 0);
	glutMotionFunc(glut_motion_func);
	glutMouseFunc(glut_mouse_func);
	glutKeyboardFunc(glut_keyboard_func);

	// ImGui Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGLUT_Init();
	//ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	// Setup Graphics
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glutMainLoop();

	// ImGui Destroy
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();


	return 0;
}