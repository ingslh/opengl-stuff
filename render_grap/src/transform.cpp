#include "transform.h"

Transform::Transform(const nlohmann::json& transform){

  for (auto& el : transform.items()) {
    readKeyframeandProperty(el.key(),el.value());
  }
}

//note: group's transform vector is 2D, and shape's transform vector is 3D
glm::vec3 Transform::GetShapeGrapOffset(){
  return std::get<t_Vector>(property_values_["Position"]) - std::get<t_Vector>(property_values_["Anchor Point"]);
}

void Transform::readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform) {
  if (propname != "Anchor Point" && propname != "Position" && propname != "Scale" && propname != "Rotation" && propname != "Opacity") return;
  bool keyvalue_is_vector = (propname == "Anchor Point" || propname == "Position" || propname == "Scale");

  auto cur_property = transform;
  if (cur_property.is_number()) { // not have keyframe
    property_values_[propname] = cur_property;
  }
  else if (cur_property.is_array()) {
    if(cur_property.size()==3)
      property_values_[propname] = glm::vec3(cur_property[0], cur_property[1], cur_property[2]);
    else
      property_values_[propname] = glm::vec3(cur_property[0], cur_property[1],0);
  }
  else if(cur_property.is_object()){ //have keyframe ,it's object
    if (keyvalue_is_vector)
      property_values_[propname] = glm::vec3(cur_property["value"][0], cur_property["value"][1], cur_property["value"][2]);
    else
      property_values_[propname] = cur_property["value"];
    VectorKeyFrames vector_keyframe;
    ScalarKeyFrames scalar_keyframe;
    for (auto it = cur_property.begin(); it != cur_property.end(); ++it) {
      if (it.key().substr(0, 8) != "keyFrame") continue;
      std::vector<float> keyframe_dirs;
      keyframe_dirs.push_back(it.value()["In influence"]);
      keyframe_dirs.push_back(it.value()["Out influence"]);
      keyframe_dirs.push_back(it.value()["In speed"]);
      keyframe_dirs.push_back(it.value()["Out speed"]);

      auto keyvalue = it.value()["keyValue"];
      if (keyvalue_is_vector) {
        auto vector_keyvalue = glm::vec3(keyvalue[0], keyvalue[1], keyvalue[2]);
        vector_keyframe.emplace_back(Keyframe<glm::vec3>(vector_keyvalue, it.value()["keyTime"], keyframe_dirs));
      }
      else {
        auto scalar_keyvalue = keyvalue;
        scalar_keyframe.emplace_back(Keyframe<float>(scalar_keyvalue, it.value()["keyTime"], keyframe_dirs));
      }
    }
    if (keyvalue_is_vector)
      keyframe_data_[propname] = vector_keyframe;
    else
      keyframe_data_[propname] = scalar_keyframe;
  }
  else 
    return;
}