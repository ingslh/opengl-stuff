#include "shapegroup.h"

ShapeGroup::ShapeGroup(const nlohmann::json& json){
  blend_mode = json["Blend Mode"];
  transform = std::make_shared<Transform>(json["Transform"]);
  contents = std::make_shared<GroupContents>(json["Contents"]);
}
