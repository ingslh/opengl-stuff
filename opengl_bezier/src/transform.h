#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Transform{
public:
  Transform(const nlohmann::json& transform);

private:
  std::vector<float> anchor_point;
  std::vector<float> position;
  std::vector<unsigned int> scale;
  unsigned int rotation;
  unsigned int opacity;
};