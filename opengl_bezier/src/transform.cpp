#include "transform.h"

Transform::Transform(const nlohmann::json& transform){

  auto size = transform["Anchor Point"].size();
  for(auto i = 0; i < size; i++){
    anchor_point.emplace_back(transform["Anchor Point"][i]);
    position.emplace_back(transform["Position"][i]);
    scale.emplace_back(transform["Scale"][i]);
  }

  rotation = transform["Rotation"];
  opacity = transform["Opacity"];
}