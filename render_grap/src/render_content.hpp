#include "layersinfo.h"
#include "Transform.h"
#include "render_data_factory.h"
#include <glm/glm.hpp>

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
  RenderContent(LayersInfo* layer_info){
    auto layer_contents_path = SRenderDataFactory::GetIns().CreateVerticesData(layer_info);
    auto layer_contents_fill = SRenderDataFactory::GetIns().CreateColorData(layer_info);
    auto layer_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(layer_info);

    auto trans_mat = layer_contents_trans->GetTransMat();

    layer_data_.index = layer_info->GetLayerInd();
    layer_data_.start_pos = trans_mat->clip_start;
    layer_data_.end_pos = trans_mat->clip_end;
    layer_data_.trans = trans_mat->trans;

    auto path_count = layer_contents_path->GetPathsCount();
    layer_data_.paths_num = path_count;
    for(unsigned int i = 0; i < path_count; i++){
      std::vector<float> vert;
      layer_contents_path->ConverToOpenglVert(i, vert);
      layer_data_.verts.emplace_back(vert);
      layer_data_.triangle_ind.emplace_back(layer_contents_path->GetTriangleIndex(i));
      layer_data_.color.emplace_back(layer_contents_fill->GetColor(i));
    }
  }


  static void MergeRenderContent(std::vector<std::shared_ptr<RenderContent>> contents);

  //use to declare VAO/VBO/EBO array
  static unsigned int GetAllPathsCount(const std::vector<std::shared_ptr<RenderContent>>& contents)){
    unsigned int count = 0;
    for(auto content : contents){
      count += content->paths_num;
    }
    return count;
  }

  static void LoopRenderContents(const std::vector<std::shared_ptr<RenderContent>>& contents, const int frames){
    auto layer_count = contents.size();
    for(auto i = 0; i < layer_count; i++){
      auto layer_data = contents[i]->GetLayerData();
      if(layer_data.start_pos < frames || layer_data.end_pos > frames) continue;
      glm::mat4 trans_mat = layer_data.trans[frames];
      //send trans_mat to shader as uniform
      for(auto j = 0; j < layer_data.paths_num; j++){
        glm::vec4 color = layer_data.color[j];
        //send color to shader as uniform
      }
    }
  }


  LayerData& GetLayerData(){return layer_data_;}
  
  
private:
  LayerData layer_data_;
};