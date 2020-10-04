#pragma once

#define PI 3.14159265358979323846
#define TAU 6.28318530717958647692
#define EPSILON 1.0E-4

void setup();
void imgui_func();
void glut_display_func();
void draw(glm::mat4);
void glut_reshape_func(int width, int height);
void glut_timer_func(int val);
void glut_motion_func(int x, int y);
void glut_mouse_func(int button, int state, int x, int y);
void glut_keyboard_func(unsigned char key, int x, int y);
