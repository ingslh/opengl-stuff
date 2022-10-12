#pragma once
#include <nlohmann/json.hpp>
#include "transform.h"
#include "groupcontents.h"

class GroupContents;
class Transform;
class ShapeGroup{
public:
  ShapeGroup(const nlohmann::json& json);
  std::shared_ptr<GroupContents> GetContents()const {return contents;}
  std::shared_ptr<Transform> GetTransform()const {return transform;}
  void CalcVertoffsetVal(const std::vector<float>& abs_anchor);

private:
  unsigned int blend_mode;
  std::shared_ptr<GroupContents> contents;
  std::shared_ptr<Transform> transform;
};