#include "transform.h"

Transform::Transform(const nlohmann::json& transform){

  auto size = transform["Anchor Point"].size();
  
  anchor_point.x = transform["Anchor Point"][0];
  anchor_point.y = transform["Anchor Point"][1];

  position.x = transform["Position"][0];
  position.y = transform["Position"][1];

  scale.x = transform["Scale"][0];
  scale.y = transform["Scale"][1];

  if (size == 3) {
    anchor_point.z = transform["Anchor Point"][2];
    position.z = transform["Position"][2];
    scale.z = transform["Scale"][2];
  }
  else {
    anchor_point.z = 0;
    position.z = 0;
    scale.z = 100;
  }

  rotation = transform["Rotation"];
  opacity = transform["Opacity"];
}

glm::vec3 Transform::GetShapeGrapOffset(){
  return position - anchor_point;
}