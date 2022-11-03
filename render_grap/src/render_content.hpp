#include "layersinfo.h"
#include "transform.h"
#include "render_data_factory.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

struct LayerData{
  unsigned int index;
  float start_pos;
  float end_pos;
  std::vector<glm::mat4> trans;

  unsigned int paths_num;
  std::vector<std::vector<float>> verts;
  std::vector<std::vector<unsigned int>> triangle_ind;

  std::vector<glm::vec4> color;
};


class RenderContent{
public:
  RenderContent(LayersInfo* layer_info);

  static unsigned int GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents);

  static unsigned int GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind);

  static void ConfigureVXOs(const std::vector<std::shared_ptr<RenderContent>>& contents);

  static void LoopRenderContents(const std::vector<std::shared_ptr<RenderContent>>& contents, const int frames);

  LayerData& GetLayerData(){return layer_data_;}
  
  
private:
  LayerData layer_data_;
};