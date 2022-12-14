#include "vertices_render_data.hpp"
#include "json_data_manager.h"
#include "shapegroup.h"
#include "transform.h"


VerticesRenderData::VerticesRenderData(const LayersInfo* data, bool out_bezier): out_bezier_(out_bezier){
  auto shape_offset = data->GetShapeTransform()->GetShapeGrapOffset();

  paths_count_ = 0;
  auto shape_groups = data->GetShapeGroup();
  for (auto group_ind = 0; group_ind < shape_groups.size(); group_ind++) {
    auto group = shape_groups[group_ind];
    auto paths = group->GetContents()->GetPaths();
    paths_count_ += static_cast<unsigned int>(paths.size());

    auto final_offset = group->GetTransform()->GetPosition() + shape_offset; 

    for(auto& path : paths) {
      if(!out_bezier_){
        bezier_vert_data_.clear();

        auto vertices = path->GetVertices();
        auto in_pos = path->GetInPosVec();
        auto out_pos = path->GetOutPosVec();

        auto vertices_count = vertices.size();
        SignalPathData signal_path;
        for(auto i = 0; i < vertices_count; i++){
          VertCluster vert_cluster;
          vert_cluster.start_pos = vertices[i] + glm::vec2(final_offset.x, final_offset.y);
          vert_cluster.out_dir = NormalizeVec2(out_pos[i] + vert_cluster.start_pos);
          vert_cluster.start_pos = NormalizeVec2(vert_cluster.start_pos);

          if(i != vertices_count - 1){
            vert_cluster.end_pos = vertices[i + 1] + glm::vec2(final_offset.x, final_offset.y);
            vert_cluster.in_dir = NormalizeVec2(in_pos[i + 1] + vert_cluster.end_pos);
          }else{
            vert_cluster.end_pos = vertices[0] + glm::vec2(final_offset.x, final_offset.y);
            vert_cluster.in_dir = NormalizeVec2(in_pos[0] + vert_cluster.end_pos);
          }
          vert_cluster.end_pos = NormalizeVec2(vert_cluster.end_pos);
          signal_path.emplace_back(vert_cluster);
        }
        multi_paths_data_.emplace_back(signal_path);
      }
      else{
        multi_paths_data_.clear();
        BezierVertData signal_path_data;
        auto bezier_verts = path->GetBezierVertices();
        auto bezier_verts_count = bezier_verts.size();
        for(auto i = 0; i < bezier_verts_count; i++){

          auto test = bezier_verts[i] + glm::vec2(final_offset.x, final_offset.y);

          auto tmp_vert = NormalizeVec2(bezier_verts[i] + glm::vec2(final_offset.x, final_offset.y));
          signal_path_data.verts.emplace_back(tmp_vert.x);
          signal_path_data.verts.emplace_back(tmp_vert.y);
          signal_path_data.verts.emplace_back(0);
        }
        signal_path_data.tri_index = path->GetTriIndexList();
        bezier_vert_data_.emplace_back(signal_path_data);
      }
    }
  }
}

unsigned int VerticesRenderData::GetVertNumByPathInd(unsigned int ind) const { 
  if(multi_paths_data_.size()){
    return static_cast<unsigned int>(multi_paths_data_[ind].size()); 
  }
  else if(bezier_vert_data_.size()){
    return static_cast<unsigned int>(bezier_vert_data_[ind].verts.size());
  }
  return 0;
}

glm::vec2 VerticesRenderData::NormalizeVec2(const glm::vec2& pos){
  auto width = JsonDataManager::GetIns().GetWidth();
  auto height = JsonDataManager::GetIns().GetHeight();
  glm::vec2 ret;
  ret.x = (pos.x - width/2) / width;
  ret.y = (pos.y - height/2) / height;
  return ret;
}

glm::vec3 VerticesRenderData::NormalizeVec3(const glm::vec3& pos){
  auto width = JsonDataManager::GetIns().GetWidth();
  auto height = JsonDataManager::GetIns().GetHeight();
  glm::vec3 ret;
  ret.x = (pos.x - width/2) / width;
  ret.y = (pos.y - height/2) / height;
  ret.z = 0;
  return ret;
}

bool VerticesRenderData::ConverToOpenglVert(unsigned int path_ind, std::vector<float>& verts){
  if(out_bezier_){
    verts = bezier_vert_data_[path_ind].verts;
  }else{
    auto vertices = multi_paths_data_[path_ind];
    verts.resize(12 * vertices.size());
    auto vert_cluster_ind = 0;
    for(auto& el : vertices){
      verts[vert_cluster_ind * 12] = el.start_pos.x;
      verts[vert_cluster_ind * 12 + 1] = el.start_pos.y;
      verts[vert_cluster_ind * 12 + 2] = 0.0f;
      verts[vert_cluster_ind * 12 + 3] = el.out_dir.x;
      verts[vert_cluster_ind * 12 + 4] = el.out_dir.y;
      verts[vert_cluster_ind * 12 + 5] = 0.0f;
      verts[vert_cluster_ind * 12 + 6] = el.in_dir.x;
      verts[vert_cluster_ind * 12 + 7] = el.in_dir.y;
      verts[vert_cluster_ind * 12 + 8] = 0.0f;
      verts[vert_cluster_ind * 12 + 9] = el.end_pos.x;
      verts[vert_cluster_ind * 12 + 10] = el.end_pos.y;
      verts[vert_cluster_ind * 12 + 11] = 0.0f;
      vert_cluster_ind++;
    }
  }
  return true;
}
