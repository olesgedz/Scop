/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jblack-b <jblack-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/19 17:57:56 by jblack-b          #+#    #+#             */
/*   Updated: 2020/03/10 20:18:06 by jblack-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESH_HPP
#define MESH_HPP
#include "scop.h"
#include <string>
#include "glm/gtx/string_cast.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.h"
#include "debug_gl.hpp"
#include "happly.h"

using namespace std;
using namespace glm;


struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh {
private:
	GLuint vbo_vertices, vbo_normals, vbo_uvs, ibo_elements;
public:
	vector<Texture> textures;
	GLuint voa, ebo;
	GLuint texture;
	GLint textureExists;
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec3> temp_normals;
	vector<glm::vec2> temp_uvs;
	Shader *shader;
	vector<GLushort> elements;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	glm::mat4 object2world;
		Mesh() : vbo_vertices(0), vbo_normals(0), ibo_elements(0), object2world(glm::mat4(1)) {
		this->textureExists = 0;
	}
	~Mesh() {
		if (vbo_vertices != 0)
			glDeleteBuffers(1, &vbo_vertices);
		if (vbo_normals != 0)
			glDeleteBuffers(1, &vbo_normals);
		if (ibo_elements != 0)
			glDeleteBuffers(1, &ibo_elements);
	}
	/**
	* Draw the object
	*/
	void draw();
	void bind_shader(Shader *shader);
	bool load_texture(const char *filename);

	bool bind_texture();

	bool load_obj(const char *filename);
    bool load_obj_ply(const char *filename);
    void make_cicle(glm::vec3 pos);
	/**
	 * Store object vertices, normals and/or elements in graphic card
	 * buffers
	 */
	void upload();


	/**
	 * Draw object bounding box
	 */
	void draw_bbox();

};

#endif