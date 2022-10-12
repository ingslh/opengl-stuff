#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include "transform.h"

struct vec2{
  float x;
  float y;
  vec2 operator+(const vec3<float>& a){
    vec2 ret;
    ret.x = this->x + a.x;
    ret.y = this->y + a.y;
    return ret;
  }

  vec2 operator+(const vec2& a){
    vec2 ret;
    ret.x = this->x + a.x;
    ret.y = this->y + a.y;
    return ret;
  }
};

class PathInfo{
public:
  PathInfo(const nlohmann::json& json);
  std::vector<vec2> GetOutPosVec(){return out_pos_vec;}
  std::vector<vec2> GetInPosVec(){return in_pos_vec;}
  std::vector<vec2> GetVertices(){return vertices;}
private:
  unsigned int shape_direction;
  std::vector<vec2> out_pos_vec;
  std::vector<vec2> in_pos_vec;
  std::vector<vec2> vertices;
  bool closed;
};