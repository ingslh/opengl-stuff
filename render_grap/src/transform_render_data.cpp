#include "transform_render_data.hpp"

TransformRenderData::TransformRenderData(const LayersInfo* layer){
  transform_mat_ = layer->GetShapeTransform()->GetTransMat();
}