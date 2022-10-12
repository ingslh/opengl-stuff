#include "transform.h"

Transform::Transform(const nlohmann::json& transform){

  auto size = transform["Anchor Point"].size();
  if(size != 3) return;
  anchor_point.x = transform["Anchor Point"][0];
  anchor_point.y = transform["Anchor Point"][1];
  anchor_point.z = transform["Anchor Point"][2];

  position.x = transform["Position"][0];
  position.y = transform["Position"][1];
  position.z = transform["Position"][2];

  scale.x = transform["Scale"][0];
  scale.y = transform["Scale"][1];
  scale.z = transform["Scale"][2];

  rotation = transform["Rotation"];
  opacity = transform["Opacity"];
}

vec3<float>& Transform::GetShapeGrapOffset(){
  return position - anchor_point;
}