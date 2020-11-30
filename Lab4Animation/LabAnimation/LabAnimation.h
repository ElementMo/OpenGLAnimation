#pragma once
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL/freeglut.h>
#include "Utils.h"
#include "Spline.h"
#include "Model.h"
#include "Boid.h"
#include "Flock.h"
#include "Octree.h"


void setup();
void imgui_func();
void glut_display_func();
void draw(glm::mat4);
void glut_reshape_func(int width, int height);
void glut_timer_func(int val);
void glut_motion_func(int x, int y);
void glut_mouse_func(int button, int state, int x, int y);
void glut_keyboard_func(unsigned char key, int x, int y);
