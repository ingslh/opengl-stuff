#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "transform.h"
#include "polygon.h"


class PathInfo{
public:
  PathInfo(const nlohmann::json& json);
  std::vector<glm::vec2> GetOutPosVec() const {return out_pos_vec;}
  std::vector<glm::vec2> GetInPosVec() const {return in_pos_vec;}
  std::vector<glm::vec2> GetVertices() const {return vertices;}
  std::vector<glm::vec2> GetBezierVertices() const {return bezier_verts_;}
  std::vector<unsigned int> GetTriIndexList() const {return tri_index_list_;}
private:
  unsigned int shape_direction;
  std::vector<glm::vec2> out_pos_vec;
  std::vector<glm::vec2> in_pos_vec;
  std::vector<glm::vec2> vertices;
  std::vector<glm::vec2> bezier_verts_;
  bool closed;
  std::shared_ptr<Polygon> polygon_;
  std::vector<unsigned int> tri_index_list_;
};