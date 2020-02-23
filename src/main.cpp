#include "shader.h"
#include "mesh.hpp"
#include "camera.hpp"
#include <string.h>
//#include <filesystem>
using namespace std;
//https://learnopengl.com/In-Practice/Debugging

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
#define GLFW_INCLUDE_NONE         // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int cursor = 0;
	// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
vector<glm::vec4> vertices;
vector<glm::vec3> normals;
vector<GLushort> elements;


// lighting
glm::vec3 lightPos(0.5f, 0.5f, 2.0f);
using namespace glm;

void load_obj(const char *filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements)
{
	ifstream in(filename, ios::in);
	if (!in)
	{
		cerr << "Cannot open " << filename << endl;
		exit(1);
	}

	string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			istringstream s(line.substr(2));
			glm::vec4 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			istringstream s(line.substr(2));
			GLushort a, b, c;
			GLushort A, B, C;
			GLushort a1, b1, c1;
			const char *chh = line.c_str();
			sscanf(chh, "f %hi/%hi/%hi %hi/%hi/%hi %hi/%hi/%hi", &a, &A, &a1, &b, &B, &b1, &c, &C, &c1);
			cout << a << " " << b << " " << c << endl;
			a--;
			b--;
			c--;
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/* ignoring this line */
		}
		else
		{
			/* ignoring this line */
		}
	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < elements.size(); i += 3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
			glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
	}
}

int main(int argc, char **argv)
{
	glfwSetErrorCallback(glfw_error_callback);
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(1);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_STICKY_KEYS,GLFW_TRUE);
	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
		bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
		bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
		bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
		bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
		bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
///////

	 // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
 	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);




	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("../shaders/shader.vs", "../shaders/shader.fs");
	//Shader lampShader("../shaders/lamp.vs", "../shaders/lamp.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	//load_obj("../african_head.obj", vertices, normals, elements);
	if (argc < 1)
		load_obj("../suzanne.obj", vertices, normals, elements);
	else
	{	char str[80];
		strcpy(str, "../");
		strcat(str, argv[1]);
		load_obj(str, vertices, normals, elements);
	}
	
	cout << "dsad" << elements.size() << endl;
	// float vertices[] = {
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

	// 	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

	// 	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};
	// first, configure the cube's VAO (and VBO)
	// unsigned int VBO, cubeVAO;
	// glGenVertexArrays(1, &cubeVAO);
	// glGenBuffers(1, &VBO);

	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// glBindVertexArray(cubeVAO);

	// // position attribute
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);

	// // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	// unsigned int lightVAO;
	// glGenVertexArrays(1, &lightVAO);
	// glBindVertexArray(lightVAO);

	// // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices.data()[0]), vertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// cout << vertices <<
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(elements.data()[0]), elements.data(), GL_STATIC_DRAW);
	// cout << "ebo" << EBO << endl;
	// GLshort indices[] = {
	// 	// note that we start from 0!
	// 	0, 1, 3, // first triangle
	// 	1, 2, 3 // second triangle
	// };
	// cout << elements.data()[0] << endl;
	// glGenBuffers(1, &EBO);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	cout << "ebo" << EBO << endl;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		// 	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// 	// -------------------------------------------------------------------------------
		// 	glfwSwapBuffers(window);
		// 	glfwPollEvents();
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Start the Dear ImGui frame

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------

		lightPos.x = 0.0f + sin(glfwGetTime() / 2) * 1.5f;
		lightPos.z = 0.0f + cos(glfwGetTime() / 2) * 1.5f;
		// lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// render the cube
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		if (EBO != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			int size;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		}
		else
		{
		//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		//cout << size << endl;
		//glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_SHORT, 0);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		 }
		// // also draw the lamp object
		// lampShader.use();
		// lampShader.setMat4("projection", projection);
		// lampShader.setMat4("view", view);
		// model = glm::mat4(1.0f);
		// model = glm::translate(model, lightPos);
		// model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		// lampShader.setMat4("model", model);
		// glBindVertexArray(lightVAO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");		   // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);			 // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	// glDeleteVertexArrays(1, &VAO);
	// // glDeleteVertexArrays(1, &lightVAO);
	// glDeleteBuffers(1, &VBO);
		ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	static float lastpress;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		float now = glfwGetTime();
		float delta = now - lastpress;
		lastpress = now;

		if (delta > 0.05)
		{
			cout << cursor << "cursor\n";

			if (!cursor)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			cursor = ~cursor;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
		if (!cursor)
		{
			camera.ProcessMouseMovement(xoffset, yoffset);
		}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}