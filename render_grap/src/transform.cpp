#include "transform.h"
#include "json_data_manager.h"

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
      property_values_[propname] = glm::vec3(cur_property[0], cur_property[1], 0);
  }
  else if(cur_property.is_object()){ //have keyframe ,it's object
    if (keyvalue_is_vector)
      property_values_[propname] = glm::vec3(cur_property["value"][0], cur_property["value"][1], cur_property["value"][2]);
    else
      property_values_[propname] = cur_property["value"];
    VectorKeyFrames vector_keyframe;
    ScalarKeyFrames scalar_keyframe;
    auto frameRate = JsonDataManager::GetIns().GetFrameRate();
    for (auto it = cur_property.begin(); it != cur_property.end(); ++it) {
      if (it.key().substr(0, 5) != "Curve") continue;
      auto lastkeyValue = it.value()["lastkeyValue"];
      auto lastkeyTime = it.value()["lastkeyTime"] * frameRate;

      glm::vec2 outPos(it.value()["OutPos"]["x"], it.value()["OutPos"]["y"]);
      glm::vec2 inPos(it.value()["InPos"]["x"], it.value()["Inpos"]["y"]);

      auto keyValue = it.value()["keyValue"];
      auto keyTime = it.value()["keyTime"] * frameRate;

      if (keyvalue_is_vector) {
        auto vector_lastkeyvalue = glm::vec3(lastkeyValue[0], lastkeyValue[1], lastkeyValue[2]);
        auto vector_keyvalue = glm::vec3(keyValue[0], keyValue[1], keyValue[2]);
        vector_keyframe.emplace_back(Keyframe<glm::vec3>(vector_lastkeyvalue, lastkeyTime, outPos, inPos, vector_keyvalue, keyTime));
      }
      else {
        auto scalar_lastkeyvalue = lastkeyValue;
        auto scalar_keyvalue = keyValue;
        scalar_keyframe.emplace_back(Keyframe<float>(scalar_lastkeyvalue, lastkeyTime, outPos, inPos, scalar_keyvalue, keyTime));
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