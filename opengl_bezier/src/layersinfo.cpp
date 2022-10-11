#include "layersinfo.h"
#include "transform.h"

LayersInfo::LayersInfo(const nlohmann::json& layer){
  index = layer["index"];
  name = layer["name"];
  blendingMode = layer["blendingMode"];
  link = layer["Parent&Link"];
  startTime = layer["startTime"];
  outPoint = layer["outPoint"];
  inPoint = layer["inPoint"];
  transform = std::make_shared<Transform>(layer["Transform"]);
}

