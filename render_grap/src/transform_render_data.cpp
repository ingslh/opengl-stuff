#include "transform_render_data.hpp"

TransformRenderData::TransformRenderData(const LayersInfo* layer){
  auto keyframes_map = layer->GetShapeTransform()->GetKeyframeData();
  if(!keyframes_map.size()) return;

  
}