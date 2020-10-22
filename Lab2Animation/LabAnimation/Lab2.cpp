#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL/freeglut.h>
#include "Utils.h"
#include "Model.h"
#include "Spline.h"
#include "Lab2.h"

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
float cam2pref = 8;
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
int ctrlPointIndex = 0;
float selectedPos[3], selectedRot[3];

// Animation Timeline
float total_t = 0.0f;
bool isPlay = true;

// Curves
Spline spline;
int curve_type_index = 0;
int orientation_scheme = 0;

float segment_t = 0;
int curve_patch = 0;


// ----------- Scene Parameters ---------
GLfloat light_position[] = { 0.0, 5.0, 0.0, 1.0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

float skeleton_col[3] = { 0.4f, 0.6f, 0.9f };
Model torso("Torso.obj", skeleton_col);
Model legLU("legLU.obj", skeleton_col);
float ground_col[3] = { 0.8f, 0.6f, 0.9f };
Model ground("ground", ground_col);
Model light_bulb("LightBulb.obj", skeleton_col);

void setup() {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);

	rotationX = 3.14;
	rotationY = 3.7;

	spline.addPoints(Vec3(-4.5f, 0.3f, -1.4f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(-2.6f, 0, -3.6f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(2, 2, -4), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(3.7f, 1, 1.0f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(1, 0.6f, 4.5f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(-1.5f, 1.3f, 2.2f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(-3.3f, 0.8f, 4.3f), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(-5.4f, 0.1f, 2.3f), EulerAngle(0, 0, 0));

	glEnable(GL_LIGHT0);

	light_bulb.setRenderMode(GL_FILL);
	torso.setRenderMode(GL_FILL);
	legLU.setRenderMode(GL_FILL);

}

void draw(glm::mat4 m_vp) {
	// Draw Ground
	ground.render(m_vp, glm::mat4(1.0), false);
	spline.render(glm::value_ptr(m_vp), ctrlPointIndex);
	light_bulb.render(m_vp, glm::translate(glm::mat4(1.0f), glm::vec3(light_position[0], light_position[1], light_position[2])), false);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	if (isPlay) {
		segment_t += deltaTime;
		total_t = curve_patch + segment_t;
	}
	if (segment_t >= 1) {

		segment_t = 0;
		++curve_patch;

		if (curve_patch >= spline.total_t) {
			curve_patch = 0;
			segment_t = 0;
			total_t = 0;
		}
	}

	m_trans[12] = spline.posAtT(segment_t, curve_patch).x;
	m_trans[13] = spline.posAtT(segment_t, curve_patch).y;
	m_trans[14] = spline.posAtT(segment_t, curve_patch).z;
	glm::mat4 m_translate = glm::make_mat4(m_trans);

	glm::mat4 m_rotation = glm::mat4(1.0f);
	if (orientation_scheme == QUATERNION) {
		m_rotation = glm::make_mat4(spline.quatAtT(segment_t, curve_patch).toRotMatrix());
	}
	else {
		m_rotation = glm::make_mat4(spline.eulerAtT(segment_t, curve_patch).toRotMatrix());
	}
	// Torso
	torso.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	torso.render(m_vp, m_translate * m_rotation, true);
	// Leg Upper Left
	legLU.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	legLU.render(m_vp , m_translate * m_rotation, true);

}

void imgui_func() {
	ImGui::Begin("Inspector", &my_tool_active, ImGuiWindowFlags_MenuBar);

	ImGui::SliderFloat("Cam FOV", &fov, 10.0f, 170.0f);
	ImGui::Checkbox("IsOrtho", &isOrtho);
	ImGui::Combo("Orientation", &orientation_scheme, "Quaternion\0Euler");

	// Curve Parameters
	ImGui::Combo("Curve Type", &curve_type_index, "CatmullRom\0B-Spline");
	spline.curveType = CURVE_TYPE(curve_type_index);
	ImGui::DragInt("Index", &ctrlPointIndex, 0.05f, 0, int(spline.ctrlPoints_pos.size()) - 1);

	// Tweak Control Points Position
	selectedPos[0] = spline.ctrlPoints_pos[ctrlPointIndex].x;
	selectedPos[1] = spline.ctrlPoints_pos[ctrlPointIndex].y;
	selectedPos[2] = spline.ctrlPoints_pos[ctrlPointIndex].z;
	ImGui::DragFloat3("Pos", selectedPos, 0.01f, -100.0f, 100.0f);
	spline.ctrlPoints_pos[ctrlPointIndex].x = selectedPos[0];
	spline.ctrlPoints_pos[ctrlPointIndex].y = selectedPos[1];
	spline.ctrlPoints_pos[ctrlPointIndex].z = selectedPos[2];

	// Tweak Control Points Rotation
	selectedRot[0] = spline.ctrlPoints_euler[ctrlPointIndex].x * RAD_TO_DEG;
	selectedRot[1] = spline.ctrlPoints_euler[ctrlPointIndex].y * RAD_TO_DEG;
	selectedRot[2] = spline.ctrlPoints_euler[ctrlPointIndex].z * RAD_TO_DEG;
	ImGui::DragFloat3("Rot", selectedRot, 1.0f, -360.0f, 360.0f);
	spline.ctrlPoints_euler[ctrlPointIndex].x = selectedRot[0] * DEG_TO_RAD;
	spline.ctrlPoints_euler[ctrlPointIndex].y = selectedRot[1] * DEG_TO_RAD;
	spline.ctrlPoints_euler[ctrlPointIndex].z = selectedRot[2] * DEG_TO_RAD;
	spline.ctrlPoints_quat[ctrlPointIndex] = fromEuler(spline.ctrlPoints_euler[ctrlPointIndex].x, spline.ctrlPoints_euler[ctrlPointIndex].y, spline.ctrlPoints_euler[ctrlPointIndex].z);

	ImGui::DragFloat3("Light Pos", light_position, 0.1f, -10.0f, 10.0f);
	ImGui::ColorPicker4("Light Ambient", light_ambient);
	ImGui::ColorPicker4("Light Diffuse", light_diffuse);
	ImGui::ColorPicker4("Light Specular", light_specular);

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

	ImGui::Begin("Timeline");
	ImGui::SetWindowPos(ImVec2(0, viewport_height - 100));
	ImGui::SetWindowSize(ImVec2(viewport_width, 100));
	if (ImGui::Button("Play/Pause")) {
		isPlay = !isPlay;
	}
	ImGui::SameLine();
	ImGui::SliderFloat("time", &total_t, 0.0f, spline.total_t);
	curve_patch = int(floor(total_t));
	segment_t = total_t - curve_patch;
	//segment_t = constrain(segment_t, 0.0f, 1.0f);
	//std::cout << segment_t << "\t" << curve_patch << "\t" << total_t << std::endl;
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
	ImGui::StyleColorsClassic();
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