#include "shader.h"
#include "mesh.hpp"
//#include <filesystem>
using namespace std;
// 32
// c++ https://opentk.net/learn/chapter1/2-hello-triangle.html

/*
	https://learnopengl.com/In-Practice/Debugging
*/
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
float u_pitch = 0.0f, u_yaw = -90.0f;
using namespace glm;

void load_obj(const char* filename, Mesh* mesh) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
  vector<int> nb_seen;

  string line;
  while (getline(in, line)) {
	if (line.substr(0,2) == "v ") {
	  istringstream s(line.substr(2));
	  glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
	  mesh->vertices.push_back(v);
	}  else if (line.substr(0,2) == "f ") {
	  istringstream s(line.substr(2));
	  GLushort a,b,c;
	  s >> a; s >> b; s >> c;
	  a--; b--; c--;
	  mesh->elements.push_back(a); mesh->elements.push_back(b); mesh->elements.push_back(c);
	}
	else if (line[0] == '#') { /* ignoring this line */ }
	else { /* ignoring this line */ }
  }

  mesh->normals.resize(mesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
  nb_seen.resize(mesh->vertices.size(), 0);
  for (unsigned int i = 0; i < mesh->elements.size(); i+=3) {
	GLushort ia = mesh->elements[i];
	GLushort ib = mesh->elements[i+1];
	GLushort ic = mesh->elements[i+2];
	glm::vec3 normal = glm::normalize(glm::cross(
	  glm::vec3(mesh->vertices[ib]) - glm::vec3(mesh->vertices[ia]),
	  glm::vec3(mesh->vertices[ic]) - glm::vec3(mesh->vertices[ia])));

	int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
	for (int j = 0; j < 3; j++) {
	  GLushort cur_v = v[j];
	  nb_seen[cur_v]++;
	  if (nb_seen[cur_v] == 1) {
	mesh->normals[cur_v] = normal;
	  } else {
	// average
	mesh->normals[cur_v].x = mesh->normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v].y = mesh->normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v].z = mesh->normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v] = glm::normalize(mesh->normals[cur_v]);
	  }
	}
  }
}

class Mesh1 {
	private:
	Shader *shader;	
	public:
	GLuint VBO;
	GLuint VAO;
	float *vertices;
	size_t nVertices;
	Mesh1(float *vertices, size_t nVertices)
	{
		this->vertices = vertices;
		this->nVertices = nVertices;
	}

	void bindShader(Shader &shader)
	{
		this->shader = &shader;
	}

	void runShader()
	{
		shader->use();
	}

	void upload()
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * nVertices, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		
		glEnableVertexAttribArray(0);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		cout << "Vertices: "<< nVertices << endl;
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * nVertices, vertices, GL_STATIC_DRAW);
		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
	}

	void draw()
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, nVertices / 3);
	}

};




int main()
{

	// glm::mat4 trans = identity<mat4>();
	// trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	// trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs");
	// float vertices[] = {
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

	// 	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	// 	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
	// 	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 0.5f, 0.0f};
	Mesh1 mesh = Mesh1(vertices, sizeof(vertices)/ sizeof(*vertices));
	mesh.upload();
	
	// unsigned int VBO;
	// glGenBuffers(1, &VBO);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// unsigned int VAO;
	// glGenVertexArrays(1, &VAO);
	// glBindVertexArray(VAO);
	// // 2. copy our vertices array in a buffer for OpenGL to use
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// // 3. then set our vertex attributes pointers
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);
	// cout << sizeof(vertices) << endl;
	// load and create a texture
	// -------------------------
	// unsigned int texture1, texture2;
	// // texture 1
	// // ---------
	// glGenTextures(1, &texture1);
	// glBindTexture(GL_TEXTURE_2D, texture1);
	// // set the texture wrapping parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// // set texture filtering parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// // load image, create texture and generate mipmaps
	// int width, height, nrChannels;
	// stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	// unsigned char *data = stbi_load("../resources/wall.jpg", &width, &height, &nrChannels, 0);
	// if (data)
	// {
	// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// 	glGenerateMipmap(GL_TEXTURE_2D);
	// }
	// else
	// {
	// 	std::cout << "Failed to load texture" << std::endl;
	// }
	// stbi_image_free(data);
	// texture 2
	// ---------
	// glGenTextures(1, &texture2);
	// glBindTexture(GL_TEXTURE_2D, texture2);
	// // set the texture wrapping parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// // set texture filtering parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// // load image, create texture and generate mipmaps
	// data = stbi_load("../resources/awesomeface.png", &width, &height, &nrChannels, 0);
	// if (data)
	// {
	// 	// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
	// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// 	glGenerateMipmap(GL_TEXTURE_2D);
	// }
	// else
	// {
	// 	std::cout << "Failed to load texture" << std::endl;
	// }
	// stbi_image_free(data);

	// glm::vec3 cubePositions[] = {
	// 	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
	// 	glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
	// 	glm::vec3(2.4f, -0.4f, -3.5f),
	// 	glm::vec3(-1.7f, 3.0f, -7.5f),
	// 	glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
	// 	glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)};

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	mesh.bindShader(ourShader);
	glEnable(GL_DEPTH_TEST);
	// // Camera
	// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	// glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	// glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	// glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	// glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	// glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	glfwSetCursorPosCallback(window, mouse_callback);
	// render loop
	// -----------
	ourShader.use();

	while (!glfwWindowShouldClose(window))
	{
	
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
		// mesh.upload();

		// activate shader
		// create transformations
	
		// render box
		mesh.draw();

	
		
		
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	// glDeleteBuffers(1, &mesh.VBO);
	//glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	u_yaw += xoffset;
	u_pitch += yoffset;
	if (u_pitch > 89.0f)
		u_pitch = 89.0f;
	if (u_pitch < -89.0f)
		u_pitch = -89.0f;
	glm::vec3 front;
	
	front.x = cos(glm::radians(u_pitch)) * cos(glm::radians(u_yaw));
	front.y = sin(glm::radians(u_pitch));
	front.z = cos(glm::radians(u_pitch)) * sin(glm::radians(u_yaw));
	cameraFront = glm::normalize(front);
}