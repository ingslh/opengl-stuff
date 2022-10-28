#pragma once
#include "base_render_data.hpp"
#include "layersinfo.h"
#include "transform.h"
#include <glm/glm.hpp>
#include <map>

class TransformRenderData : public BaseRenderData{
public:
  TransformRenderData(const LayersInfo* layer);
  TransMat* GetTransMat(){return transform_mat_;}

private:
  TransMat* transform_mat_;
  std::map<int64_t, unsigned int> opacity_map_;
};

typedef std::shared_ptr<TransformRenderData> TransformRenderDataPtr;