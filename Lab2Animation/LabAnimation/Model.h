#pragma once
#include <iostream>
#include <GL/freeglut.h>
#include "tiny_obj_loader.h"
#include "Utils.h"


class Model
{
private:
	float m_model_pos[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	float m_model_rot[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1,
		0, 0, 0, 1
	};
	const char* fileName;
	float model_color[3] = { 1, 1, 1 };
	float light_position[4];
	float light_ambient[4];
	float light_diffuse[4];
	float light_specular[4];
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	GLenum renderMode;

public:
	Model();
	Model(const char* fileName, float color[3]);
	Model(const char* fileName, const char* basePath, bool triangulate, float color[3], GLenum draw_mode);
	~Model();
	void render(const glm::mat4 m_vp, const glm::mat4 m_model, bool enable_lighting);
	void setRenderMode(GLenum renderMode);
	void setLighting(float pos[4], float amb[4], float diff[4], float spec[4]);
};