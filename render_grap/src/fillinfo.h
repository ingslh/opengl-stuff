#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <glm/glm.hpp>

class FillInfo{
public:
  FillInfo(const nlohmann::json& json);
  glm::vec4 GetColor() const {return color;}

private:
  unsigned int blend_mode;
  unsigned int composite;
  unsigned int fill_rule;
  glm::vec4 color;
  unsigned int opacity;
};