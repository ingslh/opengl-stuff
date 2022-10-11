#pragma once
#include <nlohmann/json.hpp>
#include <vector>

struct vec2{
  float x;
  float y;
};

class PathInfo{
public:
  PathInfo(const nlohmann::json& json);
private:
  unsigned int shape_direction;
  std::vector<vec2> out_pos_vec;
  std::vector<vec2> in_pos_vec;
  std::vector<vec2> vertices;
  bool closed;
};