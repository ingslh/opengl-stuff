#include "layersinfo.h"
#include "transform.h"
#include "shapegroup.h"
#include "json_data_manager.h"

LayersInfo::LayersInfo(const nlohmann::json& layer){
  index = layer["index"];
  name = layer["name"];
  blendingMode = layer["blendingMode"];
  link = layer["Parent&Link"];
  startTime = layer["startTime"];
  outPoint = layer["outPoint"];
  inPoint = layer["inPoint"];
  transform = std::make_shared<Transform>(layer["Transform"],true);
  transform->SetInandOutPos(index, inPoint, outPoint);
  transform->GenerateTransformMat();

  auto contents = layer["Contents"].items();
  for(auto& el : contents){
    groups.emplace_back(std::make_shared<ShapeGroup>(el.value()));
  }
}

