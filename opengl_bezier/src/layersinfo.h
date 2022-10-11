#pragma once
#include <nlohmann/json.hpp>
#include <string>

class ShapeGroup;
class Transform;
class LayersInfo{
public:
  LayersInfo(const nlohmann::json& layer);

private:
  unsigned int index;
  std::string name;
  unsigned int blendingMode;
  std::string link;
  float startTime;
  float outPoint;
  float inPoint;
  std::vector<std::shared_ptr<ShapeGroup>> groups;
  std::shared_ptr<Transform> transform;
};