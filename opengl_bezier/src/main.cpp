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

Camera camera(glm::vec3(0.0f, 0.0f, 0.9f));

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
  //const unsigned int render_layer = 1;
  //const unsigned int render_path = 2;

  JsonReader reader("../test.json");
  unsigned int all_vert_count = 0, vert_ind = 0;
  std::unordered_map<unsigned int, std::vector<unsigned int>> vertices_map;
  std::vector<VerticesRenderDataPtr> layers_path_data;
  auto layers_count = reader.getLayersCount();
  for (unsigned int i = 0; i < layers_count; i++) {
    auto path_data = SRenderDataFactory::GetIns().CreateVerticesData(reader.GetLayersInfo(i).get());
    auto path_count = path_data->GetMultiPathsData().size();
    layers_path_data.emplace_back(path_data);
    for (unsigned int j = 0; j < path_count; j++) {
      auto signal_path_verts_count = path_data->GetVertNumUsePathInd(j);
      all_vert_count += signal_path_verts_count;
      for (unsigned int k = 0; k < signal_path_verts_count; k++, vert_ind++) {
        std::vector<unsigned int> tmp = { i,j,k };
        vertices_map.emplace(vert_ind, tmp);
      }
    }
  }

  //auto path_data = SRenderDataFactory::GetIns().CreateVerticesData(reader.GetLayersInfo(render_layer).get());
  //const unsigned int verts_num = path_data->GetVertNumUsePathInd(render_path);

  unsigned int* VBOs = new unsigned int[all_vert_count];
  unsigned int* VAOs = new unsigned int[all_vert_count];
  glGenBuffers(all_vert_count, VBOs);
  glGenVertexArrays(all_vert_count, VAOs);


  for (auto it = vertices_map.begin(); it != vertices_map.end(); it++) {
    std::vector<float> vert;
    layers_path_data[it->second[0]]->ConverToOpenglVert(it->second[1], it->second[2], vert);
    float out_vert[12];
    memcpy(out_vert, &vert[0], vert.size() * sizeof(vert[0]));

    glBindVertexArray(VAOs[it->first]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[it->first]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(out_vert), &out_vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
  }
	
	// draw in wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

    for (unsigned int i = 0; i < all_vert_count; i++) {
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
	glDeleteVertexArrays(all_vert_count, VAOs);
	glDeleteBuffers(all_vert_count, VBOs);

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