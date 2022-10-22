#include "color_render_data.hpp"
#include "json_data_manager.h"
#include "shapegroup.h"
#include "transform.h"

ColorRenderData::ColorRenderData(const LayersInfo* data){
  fills_count_ = 0;
  auto shape_groups = data->GetShapeGroup();
  for(auto& group : shape_groups){
    auto fills = group->GetContents()->GetFills();
    fills_count_ += static_cast<unsigned int>(fills.size());
    for(auto& fill : fills){
      glm::vec4 fill_color = fill->GetColor();
      multi_fills_data_.emplace_back(fill_color);
    }
  }
}