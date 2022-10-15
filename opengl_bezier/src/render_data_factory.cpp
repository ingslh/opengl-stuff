#include "render_data_factory.h"
#include "json_data_manager.h"
#include "shapegroup.h"
#include "transform.h"
#include <mutex>

template<typename T>
void SafeDelete(BaseRenderData* data){
  auto concreate = static_cast<T*>(data);
  {
    std::lock_guard<std::recursive_mutex> guard(concreate->BaseRenderData::GetEditingLock());
    delete data;
  }
}

VerticesRenderData::VerticesRenderData(const LayersInfo* data){
  //get gobal coordation for path vertices(no keyframe)
  auto shape_offset = data->GetShapeTransform()->GetShapeGrapOffset();

  //progress layer's vector path information as vertices for opengl render
  auto shape_groups = data->GetShapeGroup();
  for(auto& group : shape_groups){
    auto paths = group->GetContents()->GetPaths();
    auto final_offset = group->GetTransform()->GetPosition() + shape_offset; 

    for(auto& path : paths){
      auto vertices = path->GetVertices();
      auto in_pos = path->GetInPosVec();
      auto out_pos = path->GetOutPosVec();
      auto vertices_count = vertices.size();
      SignalPathData signal_path;
      for(auto i = 0; i < vertices_count; i++){
        VertCluster vert_cluster;
        vert_cluster.start_pos = vertices[i] + final_offset;
        //normalizing vertices pos for render,as 0~1.
        vert_cluster.out_dir = NormalizeVec2(out_pos[i] + vert_cluster.start_pos);
        vert_cluster.start_pos = NormalizeVec2(vert_cluster.start_pos);

        if(i != vertices_count - 1){
          vert_cluster.end_pos = vertices[i + 1] + final_offset;
          vert_cluster.in_dir = NormalizeVec2(in_pos[i + 1] + vert_cluster.end_pos);
        }else{
          vert_cluster.end_pos = vertices[0] + final_offset;
          vert_cluster.in_dir = NormalizeVec2(in_pos[0] + vert_cluster.end_pos);
        }
        vert_cluster.end_pos = NormalizeVec2(vert_cluster.end_pos);
        signal_path.emplace_back(vert_cluster);
      }
      multi_paths_data_.emplace_back(signal_path);
    }
  }
}

vec2 VerticesRenderData::NormalizeVec2(const vec2& pos){
  auto width = JsonDataManager::GetIns().GetWidth();
  auto height = JsonDataManager::GetIns().GetHeight();
  vec2 ret;
  ret.x = (pos.x - width/2) / width;
  ret.y = (pos.y - height/2) / height;
  return ret;
}

bool VerticesRenderData::ConverToOpenglVert(unsigned int path_ind, unsigned int vert_ind, std::vector<float>& verts) {
  auto path = multi_paths_data_[path_ind];
  verts.resize(12);
  verts[0] = path[vert_ind].start_pos.x;
  verts[1] = path[vert_ind].start_pos.y;
  verts[2] = 0.0f;
  verts[3] = path[vert_ind].out_dir.x;
  verts[4] = path[vert_ind].out_dir.y;
  verts[5] = 0.0f;
  verts[6] = path[vert_ind].in_dir.x;
  verts[7] = path[vert_ind].in_dir.y;
  verts[8] = 0.0f;
  verts[9] = path[vert_ind].end_pos.x;
  verts[10] = path[vert_ind].end_pos.y;
  verts[11] = 0.0f;
  return true;
}

bool VerticesRenderData::ConverToOpenglVert(unsigned int path_ind, std::vector<float>& verts){
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
  return true;
}

ColorRenderData::ColorRenderData(const LayersInfo* data){

}

VerticesRenderData* RenderDataFactory::CreateVerticesData(const LayersInfo* layer){
  return new VerticesRenderData(layer);
}

ColorRenderData* RenderDataFactory::CreateColorData(const LayersInfo* layer){
  return new ColorRenderData(layer);
}

bool RenderDataFactory::ReleaseRenderData(BaseRenderData* data){
  if(!data) return false;

  switch (data->GetType())
  {
  case rDataType::tVERT_DATA:
    SafeDelete<VerticesRenderData>(data);
    break;
  case rDataType::tCOLOR_DATA:
    SafeDelete<ColorRenderData>(data);
  default:
    return false;
  }
  return true;
}