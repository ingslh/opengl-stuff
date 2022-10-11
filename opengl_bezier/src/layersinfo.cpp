#include "layersinfo.h"
#include "transform.h"
#include "shapegroup.h"

LayersInfo::LayersInfo(const nlohmann::json& layer){
  index = layer["index"];
  name = layer["name"];
  blendingMode = layer["blendingMode"];
  link = layer["Parent&Link"];
  startTime = layer["startTime"];
  outPoint = layer["outPoint"];
  inPoint = layer["inPoint"];
  transform = std::make_shared<Transform>(layer["Transform"]);
  auto contents = layer["Contents"].items();
  for(auto& el : contents){
    auto test = el.value();
    groups.emplace_back(std::make_shared<ShapeGroup>(el.value()));
  }
}

