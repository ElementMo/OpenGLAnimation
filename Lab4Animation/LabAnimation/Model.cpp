
#include "Model.h"


Model::Model() : fileName("teapot"), renderMode(GL_LINE) {
}
Model::Model(const char* fileName) : Model(fileName, NULL) {
}
Model::Model(const char* fileName, float color[3]) : Model(fileName, "models/", true, color, GL_LINE) {
}

static void PrintInfo(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials) {
	std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
	std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
		<< std::endl;

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
		printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.vertices[3 * v + 0]),
			static_cast<const double>(attrib.vertices[3 * v + 1]),
			static_cast<const double>(attrib.vertices[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
		printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.normals[3 * v + 0]),
			static_cast<const double>(attrib.normals[3 * v + 1]),
			static_cast<const double>(attrib.normals[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
		printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.texcoords[2 * v + 0]),
			static_cast<const double>(attrib.texcoords[2 * v + 1]));
	}

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", static_cast<long>(i),
			shapes[i].name.c_str());
		printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.indices.size()));
		printf("Size of shape[%ld].lines.indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].lines.indices.size()));
		printf("Size of shape[%ld].points.indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].points.indices.size()));

		size_t index_offset = 0;


		printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
				static_cast<unsigned long>(fnum));

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
					static_cast<long>(v), idx.vertex_index, idx.normal_index,
					idx.texcoord_index);
			}

			printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
				shapes[i].mesh.material_ids[f]);
			printf("  face[%ld].smoothing_group_id = %d\n", static_cast<long>(f),
				shapes[i].mesh.smoothing_group_ids[f]);

			index_offset += fnum;
		}

		printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.tags.size()));
		for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
			printf("  tag[%ld] = %s ", static_cast<long>(t),
				shapes[i].mesh.tags[t].name.c_str());
			printf(" ints: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
				printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
				if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" floats: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
				printf("%f", static_cast<const double>(
					shapes[i].mesh.tags[t].floatValues[j]));
				if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" strings: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
				printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
				if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");
			printf("\n");
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%ld].name = %s\n", static_cast<long>(i),
			materials[i].name.c_str());
		printf("  material.Ka = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].ambient[0]),
			static_cast<const double>(materials[i].ambient[1]),
			static_cast<const double>(materials[i].ambient[2]));
		printf("  material.Kd = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].diffuse[0]),
			static_cast<const double>(materials[i].diffuse[1]),
			static_cast<const double>(materials[i].diffuse[2]));
		printf("  material.Ks = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].specular[0]),
			static_cast<const double>(materials[i].specular[1]),
			static_cast<const double>(materials[i].specular[2]));
		printf("  material.Tr = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].transmittance[0]),
			static_cast<const double>(materials[i].transmittance[1]),
			static_cast<const double>(materials[i].transmittance[2]));
		printf("  material.Ke = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].emission[0]),
			static_cast<const double>(materials[i].emission[1]),
			static_cast<const double>(materials[i].emission[2]));
		printf("  material.Ns = %f\n",
			static_cast<const double>(materials[i].shininess));
		printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
		printf("  material.dissolve = %f\n",
			static_cast<const double>(materials[i].dissolve));
		printf("  material.illum = %d\n", materials[i].illum);
		printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		printf("  material.map_Ns = %s\n",
			materials[i].specular_highlight_texname.c_str());
		printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
		printf("    bump_multiplier = %f\n", static_cast<const double>(materials[i].bump_texopt.bump_multiplier));
		printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
		printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
		printf("  <<PBR>>\n");
		printf("  material.Pr     = %f\n", static_cast<const double>(materials[i].roughness));
		printf("  material.Pm     = %f\n", static_cast<const double>(materials[i].metallic));
		printf("  material.Ps     = %f\n", static_cast<const double>(materials[i].sheen));
		printf("  material.Pc     = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.Pcr    = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.aniso  = %f\n", static_cast<const double>(materials[i].anisotropy));
		printf("  material.anisor = %f\n", static_cast<const double>(materials[i].anisotropy_rotation));
		printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
		printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
		printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
		printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
		printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
		std::map<std::string, std::string>::const_iterator it(
			materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(
			materials[i].unknown_parameter.end());

		for (; it != itEnd; it++) {
			printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		}
		printf("\n");
	}
}


Model::Model(const char* fileName, const char* mtl_basePath, bool triangulate, float color[3], GLenum render_mode)
{
	this->fileName = fileName;
	this->triangulate = triangulate;

	if (color != NULL) {
		model_color[0] = color[0];
		model_color[1] = color[1];
		model_color[2] = color[2];
	}
	this->renderMode = render_mode;

	if (fileName == "teapot" || fileName == "teacup" || fileName == "ground") {
		return;
	}
	else {
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, std::string(std::string("models/") + std::string(fileName)).c_str(),
			mtl_basePath, triangulate);
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << "ERR: " << err << std::endl;
		}

		if (!ret) {
			printf("Failed to load/parse .obj file.\n");
			return;
		}

		for (int s = 0; s < shapes.size(); s++) {
			size_t v_index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				// store data
				int face_verts_num = shapes[s].mesh.num_face_vertices[f];

				glm::vec3 normal_sum = glm::vec3(0.0f);
				for (size_t v = 0; v < face_verts_num; v++) {
					size_t v_index = shapes[s].mesh.indices[v_index_offset + v].vertex_index;
					size_t n_index = shapes[s].mesh.indices[v_index_offset + v].normal_index;
					verts.push_back(glm::vec3(attrib.vertices[3 * v_index + 0], attrib.vertices[3 * v_index + 1], attrib.vertices[3 * v_index + 2]));
					normal_sum = glm::vec3(attrib.normals[3 * n_index + 0], attrib.normals[3 * n_index + 1], attrib.normals[3 * n_index + 2]);
				}
				normals.push_back(normal_sum);

				v_index_offset += face_verts_num;
			}
		}


		//std::cout << "---------------print info:---------------" << std::endl;
		//PrintInfo(attrib, shapes, materials);
	}
}


Model::~Model()
{
}

void Model::render(const glm::mat4 m_vp, const glm::mat4 m_model, bool enable_lighting)
{
	if (enable_lighting) {
		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glm::vec4 temp_light_pos = glm::inverse(m_model) * glm::make_vec4(light_position);
		float relative_light_pos[4] = {
			temp_light_pos.x,
			temp_light_pos.y,
			temp_light_pos.z,
			temp_light_pos.w,
		};
		glLightfv(GL_LIGHT0, GL_POSITION, relative_light_pos);
	}
	else {
		glDisable(GL_LIGHTING);
	}

	glLineWidth(1.0f);
	//glPushMatrix();
	glLoadMatrixf(glm::value_ptr(m_vp * m_model));
	glColor3f(model_color[0], model_color[1], model_color[2]);
	if (fileName == "teapot") {
		glutWireTeapot(1);
	}
	else if (fileName == "teacup") {
		glutWireTeacup(1);
	}
	else if (fileName == "ground") {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		int xsize = 1, zsize = 1;
		for (int j = -10; j < 10; j += zsize)
		{
			for (int i = -10; i < 10; i += xsize)
			{
				glBegin(GL_POLYGON);
				glVertex3f(i * xsize, 0.0, j * zsize);
				glVertex3f(i * xsize, 0.0, j * zsize + zsize);
				glVertex3f(i * xsize + xsize, 0.0, j * zsize + zsize);
				glVertex3f(i * xsize + xsize, 0.0, j * zsize);
				glEnd();
			}
		}
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, renderMode);
		GLfloat mat[4];

		for (int s = 0; s < shapes.size(); s++) {
			glColor3fv(materials[s].diffuse);
			// Set Material for every shape
			mat[0] = materials[s].ambient[0];
			mat[1] = materials[s].ambient[1];
			mat[2] = materials[s].ambient[2];
			mat[3] = 1.0f;
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
			mat[0] = materials[s].diffuse[0];
			mat[1] = materials[s].diffuse[1];
			mat[2] = materials[s].diffuse[2];
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
			mat[0] = materials[s].specular[0];
			mat[1] = materials[s].specular[1];
			mat[2] = materials[s].specular[2];
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
			mat[0] = materials[s].emission[0];
			mat[1] = materials[s].emission[1];
			mat[2] = materials[s].emission[2];
			glMaterialfv(GL_FRONT, GL_EMISSION, mat);
			glMaterialf(GL_FRONT, GL_SHININESS, materials[s].shininess * 128.0f);

			if (triangulate) {
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
					// Draw shape
					glBegin(GL_TRIANGLES);
					for (size_t v = 0; v < 3; v++) {
						size_t v_index = shapes[s].mesh.indices[f * 3 + v].vertex_index;
						size_t n_index = shapes[s].mesh.indices[f * 3 + v].normal_index;
						glNormal3f(attrib.normals[3 * n_index + 0], attrib.normals[3 * n_index + 1], attrib.normals[3 * n_index + 2]);
						glVertex3f(attrib.vertices[3 * v_index + 0], attrib.vertices[3 * v_index + 1], attrib.vertices[3 * v_index + 2]);
					}
					glEnd();
				}
			}
			else {
				size_t v_index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
					int face_verts_num = shapes[s].mesh.num_face_vertices[f];
					// Draw shape
					glBegin(GL_POLYGON);
					for (size_t v = 0; v < face_verts_num; v++) {
						size_t v_index = shapes[s].mesh.indices[v_index_offset + v].vertex_index;
						size_t n_index = shapes[s].mesh.indices[v_index_offset + v].normal_index;
						//std::cout << f << "--" << attrib.vertices[3 * v_index + 0] << "--" <<  attrib.vertices[3 * v_index + 1] << "--" << attrib.vertices[3 * v_index + 2] << std::endl;
						glNormal3f(attrib.normals[3 * n_index + 0], attrib.normals[3 * n_index + 1], attrib.normals[3 * n_index + 2]);
						glVertex3f(attrib.vertices[3 * v_index + 0], attrib.vertices[3 * v_index + 1], attrib.vertices[3 * v_index + 2]);
					}
					glEnd();

					//// Draw normal
					//glBegin(GL_LINES);
					//glm::vec3 vert = glm::vec3(0);
					//glm::vec3 normal = glm::vec3(0);
					//for (size_t v = 0; v < face_verts_num; v++) {
					//	size_t v_index = shapes[s].mesh.indices[v_index_offset + v].vertex_index;
					//	size_t n_index = shapes[s].mesh.indices[v_index_offset + v].normal_index;
					//	vert += glm::vec3(attrib.vertices[3 * v_index + 0], attrib.vertices[3 * v_index + 1], attrib.vertices[3 * v_index + 2]);
					//	normal += glm::vec3(attrib.normals[3 * n_index + 0], attrib.normals[3 * n_index + 1], attrib.normals[3 * n_index + 2]);
					//}
					//vert /= 3;
					//normal /= 3;
					//normal += vert;
					//glVertex3fv(glm::value_ptr(vert));
					//glVertex3fv(glm::value_ptr(normal));
					//glEnd();

					v_index_offset += face_verts_num;
				}
			}

		}
	}
	//glPopMatrix();
}

void Model::setRenderMode(GLenum render_mode)
{
	this->renderMode = render_mode;
}

void Model::setLighting(float pos[4], float amb[4], float diff[4], float spec[4])
{
	for (int i = 0; i < 4; i++) {
		light_position[i] = pos[i];
		light_ambient[i] = amb[i];
		light_diffuse[i] = diff[i];
		light_specular[i] = spec[i];
	}
}
