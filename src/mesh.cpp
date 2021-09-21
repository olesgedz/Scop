
#include "mesh.hpp"

GLint attribute_v_coord = -1;
GLint attribute_v_normal = -1;
GLint attribute_v_uvs = -1;

GLint uniform_m = -1, uniform_v = -1, uniform_p = -1;
GLint uniform_m_3x3_inv_transp = -1, uniform_v_inv = -1;
void Mesh::bind_shader(Shader *shader)
{
	this->shader = shader;
}
bool Mesh::load_texture(const char *filename)
{
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    int width, height, nrChannels;
	string s;
	s.append("./");
	s.append(filename);
	stbi_set_flip_vertically_on_load(true);
	cout <<filename <<endl;
	unsigned char *data = stbi_load(s.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		this->textureExists = 1;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	cout <<  " t1"<< texture << endl;

		stbi_image_free(data);
	return true;
}

bool Mesh::bind_texture()
{
	//this->shader->use();
	this->shader->setInt("texture1", 0);
	return true;
}

bool Mesh::load_obj_ply(const char *filename)//, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<vec2> &uvs)
{

    happly::PLYData plyIn(filename, true);

    auto  elementA_prop1 = plyIn.getElement("vertex").getPropertyNames();
    auto  modelVertices = plyIn.getVertexPositions();
    auto  modelColors = plyIn.getVertexColors();
    //auto  vertices = plyIn.getElement("vertices").getListProperty<float>();



    for(array<double, 3> ver : modelVertices)
    {
        vertices.push_back(vec4(ver[0], ver[1], ver[2], 1.0f));
    }
    for(array<unsigned char, 3> ver : modelColors)
    {
        normals.push_back(vec3(ver[0] / 255.f, ver[1] / 255.f, ver[2] / 255.f));
//        printf("%f %f %f ",ver[0] / 255.f,ver[1] / 255.f,ver[2] / 255.f);
    }


    return true;
}

bool Mesh::load_obj(const char *filename)//, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<vec2> &uvs)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1)
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		vertices.push_back(vec4(vertex, 1.0f));
	}
	cout << uvIndices.size() << endl;
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		uvs.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		normals.push_back(normal);
	}
//	cout << "Elements count: " << elements.size() << endl;

	return true;
}


	/**
	 * Store object vertices, normals and/or elements in graphic card
	 * buffers
	 */
	void Mesh::upload()
	{
		if (this->vertices.size() > 0) {
			glGenBuffers(1, &this->vbo_vertices);
			glGenVertexArrays(1, &this->voa);
			glBindVertexArray(this->voa);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices.data()[0]), vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glCheckError();
		}

		if (normals.size() > 0)
		{
			glGenBuffers(1, &vbo_normals);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]),
						normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		if (uvs.size() > 0)
		{
			if (textureExists)
			textureExists = 1;
			glGenBuffers(1, &vbo_uvs);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(uvs[0]),
						uvs.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}
		else
		{
			textureExists = 0;
		}

		if (this->vertexIndices.size() > 0)
		{
			glGenBuffers(1, &this->ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vertexIndices.size() * sizeof(this->vertexIndices[0]),
			this->vertexIndices.data(), GL_STATIC_DRAW);
		}


		// if (this->normals.size() > 0) {
		// 	glGenBuffers(1, &this->vbo_normals);
		// 	glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
		// 	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals.data()[0]),
		// 	normals.data(), GL_STATIC_DRAW);
		// 	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// 	glEnableVertexAttribArray(1);
		// 	glCheckError();
		// }
		// if (this->normalIndices.size() > 0)
		// {
		// 	glGenBuffers(1, &this->ebo);
		// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
		// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vertexIndices.size() * sizeof(this->vertexIndices[0]),
		// 	this->vertexIndices.data(), GL_STATIC_DRAW);
		// }



	}

	/**
	 * Draw the object
	 */
	void Mesh::draw()
	{

		if (this->vbo_vertices != 0) {
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
			glVertexAttribPointer(
				0,  // attribute
				3,                  // number of elements per vertex, here (x,y,z,w)
				GL_FLOAT,           // the type of each element
				GL_FALSE,           // take our values as-is
				0,                  // no extra data between each position
				0                   // offset of first element
			);
		}

		if (this->vbo_normals != 0) {
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
			glVertexAttribPointer(
				1, // attribute
				3,                  // number of elements per vertex, here (x,y,z)
				GL_FLOAT,           // the type of each element
				GL_FALSE,           // take our values as-is
				0,                  // no extra data between each position
				0                   // offset of first element
			);
		}


		if (this->vbo_uvs != 0) {
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			
		}
		


		glBindVertexArray(this->voa);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		if (false && this->vertexIndices.size() > 0)
		{
//			glDrawElements(GL_TRIANGLES, this->vertexIndices.size(), GL_UNSIGNED_INT, 0);
            glDrawElements(GL_TRIANGLES, this->vertexIndices.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
//			glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
            glDrawArrays(GL_POINTS, 0, this->vertices.size());
		}
		
		
		if (this->vbo_normals != 0)
			glDisableVertexAttribArray(attribute_v_normal);
		if (this->vbo_vertices != 0)
			glDisableVertexAttribArray(attribute_v_coord);
		if (this->vbo_vertices != 0)
			glDisableVertexAttribArray(attribute_v_uvs);
		if (this->vbo_uvs != 0)
			glDisableVertexAttribArray(attribute_v_uvs);
	}

	/**
	 * Draw object bounding box
	 */
	void Mesh::draw_bbox() {
		if (this->vertices.size() == 0)
			return;
		
		// Cube 1x1x1, centered on origin
		GLfloat vertices[] = {
			-0.5, -0.5, -0.5, 1.0,
			 0.5, -0.5, -0.5, 1.0,
			 0.5,  0.5, -0.5, 1.0,
			-0.5,  0.5, -0.5, 1.0,
			-0.5, -0.5,  0.5, 1.0,
			 0.5, -0.5,  0.5, 1.0,
			 0.5,  0.5,  0.5, 1.0,
			-0.5,  0.5,  0.5, 1.0,
		};
		GLuint vbo_vertices;
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLushort elements[] = {
			0, 1, 2, 3,
			4, 5, 6, 7,
			0, 4, 1, 5, 2, 6, 3, 7
		};
		GLuint ibo_elements;
		glGenBuffers(1, &ibo_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		GLfloat
			min_x, max_x,
			min_y, max_y,
			min_z, max_z;
		min_x = max_x = this->vertices[0].x;
		min_y = max_y = this->vertices[0].y;
		min_z = max_z = this->vertices[0].z;
		for (unsigned int i = 0; i < this->vertices.size(); i++) {
			if (this->vertices[i].x < min_x) min_x = this->vertices[i].x;
			if (this->vertices[i].x > max_x) max_x = this->vertices[i].x;
			if (this->vertices[i].y < min_y) min_y = this->vertices[i].y;
			if (this->vertices[i].y > max_y) max_y = this->vertices[i].y;
			if (this->vertices[i].z < min_z) min_z = this->vertices[i].z;
			if (this->vertices[i].z > max_z) max_z = this->vertices[i].z;
		}
		glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
		glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
		glm::mat4 transform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);
		
		/* Apply object's transformation matrix */
		glm::mat4 m = this->object2world * transform;
		glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glEnableVertexAttribArray(attribute_v_coord);
		glVertexAttribPointer(
			attribute_v_coord,  // attribute
			3,                  // number of elements per vertex, here (x,y,z,w)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glDisableVertexAttribArray(attribute_v_coord);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDeleteBuffers(1, &vbo_vertices);
		glDeleteBuffers(1, &ibo_elements);
	}
