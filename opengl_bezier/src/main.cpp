#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.h"
#include "shader_g.h"
#include "jsonreader.h"
#include "render_data_factory.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader shader("../shader_bezier.vs", "../shader_bezier.fs", "../shader_bezier.gs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
  const unsigned int render_layer = 1;
  const unsigned int render_path = 2;

  JsonReader reader("../test.json");
  auto path_data = SRenderDataFactory::GetIns().CreateVerticesData(reader.GetLayersInfo(render_layer).get());
  const unsigned int verts_num = path_data->GetVertNumUsePathInd(render_path);

  unsigned int* VBOs = new unsigned int[verts_num];
  unsigned int* VAOs = new unsigned int[verts_num];
  glGenBuffers(verts_num, VBOs);
  glGenVertexArrays(verts_num, VAOs);

  for (unsigned int i = 0; i < verts_num; i++) {
    std::vector<float> vert;
    path_data->ConverToOpenglVert(render_path, i, vert);
    float out_vert[12];
    memcpy(out_vert, &vert[0], vert.size() * sizeof(vert[0]));

    glBindVertexArray(VAOs[i]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(out_vert), &out_vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
  }

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw points
		shader.use();
		glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		//根据ae中的path数据进行贝塞尔曲线绘制，每个曲线需要四个顶点
    for (unsigned int i = 0; i < verts_num; i++) {
      glBindVertexArray(VAOs[i]);
      glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
    }

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(verts_num, VAOs);
	glDeleteBuffers(verts_num, VBOs);

	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}