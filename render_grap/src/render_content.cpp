#include "render_content.hpp"

RenderContent::RenderContent(LayersInfo* layer_info){
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

unsigned int RenderContent::GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents) {
  unsigned int count = 0;
  for (auto content : contents) {
    count += content->GetLayerData().paths_num;
  }
  return count;
}

unsigned int RenderContent::GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind) {
  unsigned int index = 0;
  if (layer_ind == 0)
    return path_ind;
  else {
    for (auto i = 0; i <= layer_ind - 1; i++) {
      index += contents[i]->GetLayerData().paths_num;
    }
    return index + path_ind;
  }
}

void RenderContent::ConfigureVXOs(const std::vector<std::shared_ptr<RenderContent>>& contents){
  unsigned int count = 0;
  for(auto content : contents){
    count += content->GetLayerData().paths_num;
  }

  unsigned int* VBOs = new unsigned int[count];
  unsigned int* VAOs = new unsigned int[count];
  unsigned int* EBOs = new unsigned int[count];
  glGenBuffers(count, VBOs);
  glGenBuffers(count, EBOs);
  glGenVertexArrays(count, VAOs);
  unsigned int index = 0;

  for(auto i = 0; i < contents.size(); i++){
    for(auto j = 0; j < contents[i]->GetLayerData().paths_num; j++){
      index++;
      auto vert_array = contents[i]->GetLayerData().verts[j];
      auto out_vert = new float[vert_array.size()];
      memcpy(out_vert, &vert_array[0], vert_array.size() * sizeof(vert_array[0]));

      auto tri_array = contents[i]->GetLayerData().triangle_ind[j];
      auto out_tri_ind = new unsigned int[tri_array.size()];
      memcpy(out_tri_ind, &tri_array[0], tri_array.size() * sizeof(tri_array[0]));

      glBindVertexArray(VAOs[index]);
      glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert_array.size(), out_vert, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tri_array.size(), out_tri_ind, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
      glEnableVertexAttribArray(0);
    }
  }
}

void RenderContent::LoopRenderContents(const std::vector<std::shared_ptr<RenderContent>>& contents, const int frames){
  auto layer_count = contents.size();
  unsigned int index = 0;
  for(auto i = 0; i < layer_count; i++){
    auto layer_data = contents[i]->GetLayerData();
    if(layer_data.start_pos < frames || layer_data.end_pos > frames) continue;
    glm::mat4 trans_mat = layer_data.trans[frames];
    //send trans_mat to shader as uniform
    for(auto j = 0; j < layer_data.paths_num; j++){
      glm::vec4 color = layer_data.color[j];
      //send color to shader as uniform
      //glBindVertexArray(VAOs[index]);
      glDrawElements(GL_TRIANGLES, layer_data.verts[j].size(), GL_UNSIGNED_INT, 0);
    }
  }
}