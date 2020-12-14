#define TINYOBJLOADER_IMPLEMENTATION


#include "LabAnimation.h"

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
bool mousePressed = false;
bool is_hovering = false;

// ----------- Camera Parameters -------
float rotationX, rotationY;
float fov = 75;
float cam2pref = 45;
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
bool showLight;
GLfloat light_position[] = { -10.0, 10.0, -10.0, 1.0 };
GLfloat light_ambient[] = { 0.012, 0.043, 0.141, 1.0 };
GLfloat light_diffuse[] = { 1.0, 0.98, 0.93, 1.0 };
GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat bg_color[] = { 0.2, 0.31, 0.42, 1.0 };

Model boid_model_1("boid.obj");
Model boid_model_2("fish1.obj");
Model shark_model("shark.obj");
float ground_col[3] = { 0.6f, 0.5f, 0.3f };
Model ground("ground", ground_col);
Model light_bulb("LightBulb.obj");


// Animation Timeline
float total_t = 0.0f;
bool isPlay = true;

// Curve
bool show_timeline = false;
bool show_curve = false;
Spline spline;
float segment_t = 0;
int curve_patch = 0;
int ctrlPointIndex = 0;
float selectedPos[3], selectedRot[3];;

// ----------- Flock World -------------
Flock flock(30.0f);
bool showOctree;
std::vector<Model> seaweeds;
std::vector<glm::vec3> seaweeds_pos;
glm::vec3 predator_prepos;

float param1 = 5.0f;
float param2 = 0.25f;
float param3 = 0.04f;
float param4 = 0;
float param_offset_1[3] = { 1,0,0 };
float param_offset_2[3] = { 1,1,1 };


void setup() {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);

	rotationX = -0.27;
	rotationY = 12.73;

	glEnable(GL_LIGHT0);

	light_bulb.setRenderMode(GL_FILL);

	boid_model_1.setRenderMode(GL_FILL);
	boid_model_2.setRenderMode(GL_FILL);
	shark_model.setRenderMode(GL_FILL);

	for (int i = 0; i < 500; i++)
	{
		if (i % 2 == 0) {
			Boid b(boid_model_1, Random(-30, 30), Random(-15, 15), Random(-30, 30));
			flock.AddBoid(b);
		}
		if (i % 2 == 1) {
			Boid b(boid_model_2, Random(-30, 30), Random(-15, 15), Random(-30, 30));
			flock.AddBoid(b);
		}
	}

	flock.predators.push_back(glm::vec3(0));

	// Set predator curve
	spline.curveType = CATMULL_ROM;
	spline.addPoints(Vec3(-52.0f, 0.3f, -15.0f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(-50.0f, 0.0f, -24.6f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(1.0f, 0.0f, -18.0f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(45.0f, 1.0f, 13.0f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(1.0f, 0.6f, 50.2f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(-7.5f, 9.3f, 11.2f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(-47.3f, 0.8f, -13.0f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(-50.0f, 0.0f, -24.6f), EulerAngle(0.0f, 0.0f, 0.0f));
	spline.addPoints(Vec3(-52.0f, 0.3f, -15.0f), EulerAngle(0.0f, 0.0f, 0.0f));


	for (int i = 0; i < 10; i++)
	{
		Model sw("seaweed.obj");
		sw.setRenderMode(GL_FILL);
		seaweeds.push_back(sw);
		seaweeds_pos.push_back(glm::vec3(Random(-50, 50), -15, Random(-50, 50)));
	}
}


void draw(glm::mat4 m_vp) {
	glClearColor(bg_color[0], bg_color[1], bg_color[2], 0.0f);

	std::cout << "X: " << rotationX << " Y: " << rotationY << " D: " << cam2pref << std::endl;

	if (showLight) {
		// Draw Ground
		glm::mat4 m_ground = glm::translate(glm::mat4(1.0f), glm::vec3(0, -15, 0));
		ground.render(m_vp, m_ground, false);
		light_bulb.render(m_vp, glm::translate(glm::mat4(1.0f), glm::vec3(light_position[0], light_position[1], light_position[2])), false);

	}

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Update Flock World Lighting
	flock.SetLighting(light_position, light_ambient, light_diffuse, light_specular);

	// Simulate Flock
	flock.Simulate();

	// Redner Flock
	flock.Render(m_vp, showOctree);

	// Set Boid parameters
	flock.SetRadius(param1);
	flock.SetMaxSpeed(param2);
	flock.SetMaxForce(param3);


	// Render Predator
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
	if (show_curve) {
		spline.render(glm::value_ptr(m_vp), ctrlPointIndex);
	}

	flock.predators[0].x = spline.posAtT(segment_t, curve_patch).x;
	flock.predators[0].y = spline.posAtT(segment_t, curve_patch).y;
	flock.predators[0].z = spline.posAtT(segment_t, curve_patch).z;
	glm::vec3 predator_vel = flock.predators[0] - predator_prepos;
	glm::mat4 m_predator_translate = glm::translate(glm::mat4(1.0f), flock.predators[0]);
	glm::mat4 m_predator_rotation = glm::inverse(glm::lookAt(glm::vec3(0), glm::normalize(predator_vel), glm::vec3(0, 1, 0)));
	shark_model.setLighting(light_position, light_ambient, light_diffuse, light_specular);
	shark_model.render(m_vp, m_predator_translate * m_predator_rotation, true);

	predator_prepos = flock.predators[0];

	for (int i = 0; i < 10; i++) {
		glm::mat4 m_seaweed = glm::translate(glm::mat4(1.0f), seaweeds_pos[i]);
		seaweeds[i].setLighting(light_position, light_ambient, light_diffuse, light_specular);
		seaweeds[i].render(m_vp, m_seaweed, true);
	}
}

void imgui_func() {
	ImGui::Begin("Inspector", &my_tool_active, ImGuiWindowFlags_MenuBar);

	ImGui::SliderFloat("Cam FOV", &fov, 10.0f, 170.0f);
	ImGui::Checkbox("IsOrtho", &isOrtho);

	ImGui::SliderFloat("Radius", &param1, 3.0f, 9.0f);
	ImGui::SliderFloat("MaxSpeed", &param2, 0.01f, 1.0f);
	ImGui::SliderFloat("MaxSteerForce", &param3, 0.001f, 0.5f);
	//ImGui::DragFloat3("Param1", param_offset_1, 0.1f, -10.0f, 10.0f);
	//ImGui::DragFloat3("Param2", param_offset_2, 0.1f, -10.0f, 10.0f);

	ImGui::Checkbox("Show Octree", &showOctree);

	ImGui::Checkbox("Show Curve", &show_curve);
	ImGui::Checkbox("Show Timeline", &show_timeline);
	ImGui::DragInt("Index", &ctrlPointIndex, 0.05f, 0, int(spline.ctrlPoints_pos.size()) - 1);
	// Tweak Control Points Position
	selectedPos[0] = spline.ctrlPoints_pos[ctrlPointIndex].x;
	selectedPos[1] = spline.ctrlPoints_pos[ctrlPointIndex].y;
	selectedPos[2] = spline.ctrlPoints_pos[ctrlPointIndex].z;
	ImGui::DragFloat3("Pos", selectedPos, 0.1f, -100.0f, 100.0f);
	spline.ctrlPoints_pos[ctrlPointIndex].x = selectedPos[0];
	spline.ctrlPoints_pos[ctrlPointIndex].y = selectedPos[1];
	spline.ctrlPoints_pos[ctrlPointIndex].z = selectedPos[2];

	//// Tweak Control Points Rotation
	//selectedRot[0] = spline.ctrlPoints_euler[ctrlPointIndex].x * RAD_TO_DEG;
	//selectedRot[1] = spline.ctrlPoints_euler[ctrlPointIndex].y * RAD_TO_DEG;
	//selectedRot[2] = spline.ctrlPoints_euler[ctrlPointIndex].z * RAD_TO_DEG;
	//ImGui::DragFloat3("Rot", selectedRot, 1.0f, -360.0f, 360.0f);
	//spline.ctrlPoints_euler[ctrlPointIndex].x = selectedRot[0] * DEG_TO_RAD;
	//spline.ctrlPoints_euler[ctrlPointIndex].y = selectedRot[1] * DEG_TO_RAD;
	//spline.ctrlPoints_euler[ctrlPointIndex].z = selectedRot[2] * DEG_TO_RAD;
	//spline.ctrlPoints_quat[ctrlPointIndex] = fromEuler(spline.ctrlPoints_euler[ctrlPointIndex].x, spline.ctrlPoints_euler[ctrlPointIndex].y, spline.ctrlPoints_euler[ctrlPointIndex].z);



	ImGui::DragFloat3("Light Pos", light_position, 0.1f, -100.0f, 100.0f);
	ImGui::Checkbox("Show Light", &showLight);
	ImGui::ColorPicker4("Light Ambient", light_ambient);
	ImGui::ColorPicker4("Light Diffuse", light_diffuse);
	ImGui::ColorPicker4("Light Specular", light_specular);
	ImGui::ColorPicker4("Backgound Color", bg_color);
	if (ImGui::Button("Reset")) {
		flock.Reset();
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

	if (show_timeline) {
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
	rotationY += (mouseY - pmouseY) * 0.05f;

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
		mousePressed = true;
	}
	else if (state == GLUT_UP) {
		mousePressed = false;
	}
	if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) {
			return;
		}
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
	glutCreateWindow("ComputerAnimation");

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
