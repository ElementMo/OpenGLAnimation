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
float cam2pref = 11;
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
bool show_curve = false;
Spline spline;
int curve_type_index = 0;
int orientation_scheme = 0;

float segment_t = 0;
int curve_patch = 0;


// ----------- Scene Parameters ---------
GLfloat light_position[] = { -6.0, 4.0, 2.0, 1.0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

float skeleton_col[3] = { 0.4f, 0.6f, 0.9f };
Model torso("Torso.obj", skeleton_col);
Model legLU("legLU.obj", skeleton_col);
Model legLD("legLD.obj", skeleton_col);
Model legRU("legRU.obj", skeleton_col);
Model legRD("legRD.obj", skeleton_col);
Model footL("FootL.obj", skeleton_col);
Model footR("FootR.obj", skeleton_col);
float ground_col[3] = { 0.8f, 0.6f, 0.9f };
Model ground("ground", ground_col);
Model light_bulb("LightBulb.obj", skeleton_col);


float walk_speed = 3.6;
float param1 = 2;
float param2 = 15;
float param3 = 15;
float param4 = 15;
float param_offset[3] = { 0.0 };

void setup() {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);

	rotationX = 3.55;
	rotationY = 5.67;

	spline.addPoints(Vec3(0, 3.5, -9), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, -6), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, -3), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, 0), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, 3), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, 6), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, 9), EulerAngle(0, 0, 0));
	spline.addPoints(Vec3(0, 3.5, 12), EulerAngle(0, 0, 0));

	glEnable(GL_LIGHT0);

	light_bulb.setRenderMode(GL_FILL);
	torso.setRenderMode(GL_FILL);
	legLU.setRenderMode(GL_FILL);
	legLD.setRenderMode(GL_FILL);
	legRU.setRenderMode(GL_FILL);
	legRD.setRenderMode(GL_FILL);
	footL.setRenderMode(GL_FILL);
	footR.setRenderMode(GL_FILL);

}

float footAngle(float _x) {
	float x = 2 * _x;
	return (sin(x) - 0.5 * sin(2 * x) + 0.3 * sin(3 * x) - 0.25 * sin(4 * x) + 0.1 * sin(5 * x));
}

void draw(glm::mat4 m_vp) {
	//std::cout << rotationX << "--" << rotationY << std::endl;
	// Draw Ground
	ground.render(m_vp, glm::mat4(1.0), false);
	if (show_curve) {
		spline.render(glm::value_ptr(m_vp), ctrlPointIndex);
	}
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
	glm::mat4 m_torso = m_translate * m_rotation;
	torso.render(m_vp, m_torso, true);

	// Leg Upper Left
	float leg_rotate = pow((sin(frameCount * DEG_TO_RAD * walk_speed)), 1) * 24 + -3;
	glm::mat4 m_leg_rotate = glm::rotate(glm::mat4(1.0f), leg_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	glm::mat4 m_leg_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.02f, 0.01f));
	legLU.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	glm::mat4 m_legLU = m_torso * m_leg_offset * m_leg_rotate;
	legLU.render(m_vp, m_legLU, true);

	// Leg Downer Left
	leg_rotate = abs(pow((sin((frameCount * DEG_TO_RAD * walk_speed) - PI / 4)), 3)) * 35 + -7;
	m_leg_rotate = glm::rotate(glm::mat4(1.0f), leg_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	m_leg_offset = glm::translate(glm::mat4(1.0f), glm::vec3(-0.06f, -1.61f, 0));
	legLD.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	glm::mat4 m_legLD = m_legLU * m_leg_offset * m_leg_rotate;
	legLD.render(m_vp, m_legLD, true);

	// Right Upper Left
	leg_rotate = pow((sin(frameCount * DEG_TO_RAD * walk_speed + PI)), 1) * 24 + -3;
	m_leg_rotate = glm::rotate(glm::mat4(1.0f), leg_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	m_leg_offset = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, 0.02f, 0.01f));
	legRU.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	glm::mat4 m_legRU = m_torso * m_leg_offset * m_leg_rotate;
	legRU.render(m_vp, m_legRU, true);

	// Right Downer Left
	leg_rotate = abs(pow((sin((frameCount * DEG_TO_RAD * walk_speed) - PI / 4)), 3)) * 35 + -7;
	m_leg_rotate = glm::rotate(glm::mat4(1.0f), leg_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	m_leg_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.72, -0.17));
	legRD.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	glm::mat4 m_legRD = m_legRU * m_leg_offset * m_leg_rotate;
	legRD.render(m_vp, m_legRD, true);

	// Foot Left
	float foot_rotate = footAngle(frameCount * DEG_TO_RAD * walk_speed) * 4.4 - 1.7;
	glm::mat4 m_foot_rotate = glm::rotate(glm::mat4(1.0f), foot_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	glm::mat4 m_foot_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, -0.36));
	footL.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	footL.render(m_vp, m_legLD * m_foot_offset * m_foot_rotate, true);

	// Foot Right
	foot_rotate = footAngle(frameCount * DEG_TO_RAD * walk_speed) * 4.4 - 1.7;
	m_foot_rotate = glm::rotate(glm::mat4(1.0f), foot_rotate * DEG_TO_RAD, glm::vec3(1, 0, 0));
	m_foot_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, -0.36));
	footL.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	footL.render(m_vp, m_legRD * m_foot_offset * m_foot_rotate, true);

}

void imgui_func() {
	ImGui::Begin("Inspector", &my_tool_active, ImGuiWindowFlags_MenuBar);

	ImGui::SliderFloat("Cam FOV", &fov, 10.0f, 170.0f);
	ImGui::Checkbox("IsOrtho", &isOrtho);
	ImGui::Combo("Orientation", &orientation_scheme, "Quaternion\0Euler");

	// Curve Parameters
	ImGui::Checkbox("Show Curve", &show_curve);
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

	ImGui::DragFloat("Walk Speed", &walk_speed, 0.1f);

	//ImGui::DragFloat("Param1", &param1, 0.1f);
	//ImGui::DragFloat("Param2", &param2, 0.1f);
	//ImGui::DragFloat("Param3", &param3, 0.1f);
	//ImGui::DragFloat("Param4", &param4, 0.1f);
	//ImGui::DragFloat3("Param Offset", param_offset, 0.01f);

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
	glClearColor(0.05f, 0.05f, 0.05f, 0.0f);
	glutMainLoop();

	// ImGui Destroy
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}