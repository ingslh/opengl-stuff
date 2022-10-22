#pragma once
#include "base_render_data.hpp"
#include "layersinfo.h"
#include <vector>
#include <glm/glm.hpp>


class ColorRenderData : public BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
  glm::vec4 GetColor(unsigned int ind) { return multi_fills_data_[ind]; }
private:
  unsigned int fills_count_;
  std::vector<glm::vec4> multi_fills_data_;
};

typedef std::shared_ptr<ColorRenderData> ColorRenderDataPtr;