#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.h"
#include "shader_g.h"
#include "jsonreader.h"
#include "render_data_factory.h"
#include "render_content.hpp"

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
	//Shader shader("../shader_bezier.vert", "../shader_bezier.frag", "../shader_bezier.geom");
	Shader shader("../shader_bezier.vert", "../shader_bezier.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

  JsonReader reader("../test.json");
  unsigned int path_ind = 0;

  std::vector<VerticesRenderDataPtr> layers_path_data;

	std::map<unsigned int, std::vector<unsigned int>> paths_map;//all_path_ind;{layer_ind, path_ind, vert_count, triangle_count}
  std::map<unsigned int, glm::vec4> fills_map;//all_fill_ind; color
  std::vector<TransMat*> trans_mat_list;

  auto layers_count = reader.getLayersCount();
  for (unsigned int i = 0; i < layers_count; i++) {
    auto layer_info = reader.GetLayersInfo(i).get();

    auto layer_contents_path = SRenderDataFactory::GetIns().CreateVerticesData(layer_info);
		layers_path_data.emplace_back(layer_contents_path);

    auto layer_contents_fill = SRenderDataFactory::GetIns().CreateColorData(layer_info);

    auto layer_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(layer_info);
    trans_mat_list.emplace_back(layer_contents_trans->GetTransMat());


    auto path_count = layer_contents_path->GetPathsCount();
    for (unsigned int j = 0; j < path_count; j++, path_ind++) {
      auto signal_path_verts_count = layer_contents_path->GetVertNumByPathInd(j);
			auto indices_count = layer_contents_path->GetTriangleIndexSize(j);
			std::vector<unsigned int> path_tmp ={i, j, signal_path_verts_count, indices_count};
			paths_map.emplace(path_ind, path_tmp);

      auto fill_color = layer_contents_fill->GetColor(j);
      fills_map.emplace(path_ind, fill_color);

    }
  }
	auto paths_count = path_ind;

  unsigned int* VBOs = new unsigned int[paths_count];
  unsigned int* VAOs = new unsigned int[paths_count];
	unsigned int* EBOs = new unsigned int[paths_count];
  glGenBuffers(paths_count, VBOs);
	glGenBuffers(paths_count, EBOs);
  glGenVertexArrays(paths_count, VAOs);


  for (auto it = paths_map.begin(); it != paths_map.end(); it++) {
    std::vector<float> vert;
		std::vector<unsigned int> tri_index;
		layers_path_data[it->second[0]] -> ConverToOpenglVert(it->second[1], vert);

		unsigned int vert_array_size = 0;
		if(layers_path_data[it->second[0]]->GetOutBezier()){
			vert_array_size = it->second[2];

			tri_index = layers_path_data[it->second[0]]->GetTriangleIndex(it->second[1]);
		}else{
			vert_array_size = 12 * it->second[2];
		}

    auto out_vert = new float[vert_array_size];
    memcpy(out_vert, &vert[0], vert.size() * sizeof(vert[0]));

		auto indices = new unsigned int[tri_index.size()];
		memcpy(indices, &tri_index[0], tri_index.size() * sizeof(tri_index[0]));

    glBindVertexArray(VAOs[it->first]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[it->first]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * it->second[2], out_vert, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[it->first]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tri_index.size(), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
  }

  glfwSwapInterval(1);// open the vertical synchronization
  // draw points
  shader.use();
  glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  glm::mat4 view = camera.GetViewMatrix();
  glm::mat4 model = glm::mat4(1.0f);
  shader.setMat4("projection", projection);
  shader.setMat4("view", view);
  shader.setMat4("model", model);
	
	// draw in wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  static double limitFPS = 1.0 / 60.0;
  double lastTime = glfwGetTime(), timer = lastTime;
  double deltaTime = 0, nowTime = 0;
  int frames = 0, played_frames = 0;;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
    // - Measure time
    nowTime = glfwGetTime();
    deltaTime += (nowTime - lastTime) / limitFPS;
    lastTime = nowTime;

    while (deltaTime >= 1.0) { // render
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (unsigned int i = 0; i < paths_count; i++) {
        shader.setVec4("Color", fills_map[i]);
        //shader.setMat4("Transform", transforms_mat[i][played_frames]); //
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, paths_map[i][3], GL_UNSIGNED_INT, 0);
      }

      

      
      frames++;
      deltaTime--;
    }

    // - Reset after one second
    if (glfwGetTime() - timer > 1.0) {
      timer++;
      std::cout << "FPS: " << frames  << std::endl;
      frames = 0;
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(paths_count, VAOs);
	glDeleteBuffers(paths_count, VBOs);
	glDeleteBuffers(paths_count, EBOs);

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