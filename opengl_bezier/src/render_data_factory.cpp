#include "render_data_factory.h"
#include "json_data_manager.h"
#include "shapegroup.h"
#include "transform.h"
#include <mutex>

template<typename T>
void SafeDelete(BaseRenderData* data){
  auto concreate = static_cast<T*>(data);
  {
    std::lock_guard<std::recursive_mutex> guard(concreate->GetEditinglock());
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

  //normalizing vertices pos for render,as 0~1.
}

vec2 VerticesRenderData::NormalizeVec2(const vec2& pos){
  auto width = JsonDataManager::GetIns().GetWidth();
  auto height = JsonDataManager::GetIns().GetHeight();
  vec2 ret;
  ret.x = pos.x / width;
  ret.y = pos.y / height;
  return ret;
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
}