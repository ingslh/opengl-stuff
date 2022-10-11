#pragma once
#include <nlohmann/json.hpp>
#include <string>

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
  //Contents_vector<group>
  std::shared_ptr<Transform> transform;
};