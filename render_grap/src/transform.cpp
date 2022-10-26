#include "transform.h"
#include "json_data_manager.h"
#include "bezier_generator.hpp"

Transform::Transform(const nlohmann::json& transform, bool IsShapeTransform){

  //1.read layer's transofrm infos
  for (auto& el : transform.items()) {
    readKeyframeandProperty(el.key(),el.value());
  }
  if (!IsShapeTransform) return;
  //2.generate bezier curve data
  for (auto it = keyframe_data_.begin(); it != keyframe_data_.end(); it++) {
    if (IsVectorProperty(it->first)) {
      auto vector_keyframes = std::get<0>(it->second);
      auto start_value = std::get<0>(property_values_[it->first]);
      auto start = vector_keyframes.front().lastkeyTime;

      for (auto& keyframe : vector_keyframes) {
        unsigned int bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos_x(keyframe.lastkeyTime, keyframe.lastkeyValue.x);
        glm::vec2 lastPos_y(keyframe.lastkeyTime, keyframe.lastkeyValue.y);
        glm::vec2 outPos_x(keyframe.outPos[0].x, keyframe.outPos[0].y);
        glm::vec2 outPos_y(keyframe.outPos[1].x, keyframe.outPos[1].y);
        glm::vec2 inPos_x(keyframe.inPos[0].x, keyframe.inPos[0].y);
        glm::vec2 inPos_y(keyframe.inPos[1].x, keyframe.inPos[1].y);
        glm::vec2 curPos_x(keyframe.keyTime, keyframe.keyValue.x);
        glm::vec2 curPos_y(keyframe.keyTime, keyframe.keyValue.y);
        BezierGenerator generator_x(lastPos_x, outPos_x, inPos_x, curPos_x, bezier_duration, start);
        auto curve_x = generator_x.getKeyframeCurve();
        BezierGenerator generator_y(lastPos_y, outPos_x, inPos_x, curPos_x, bezier_duration, start);
        auto curve_y = generator_y.getKeyframeCurve();
        start += curve_x.size(); 
      }
    }
    else {
      auto scalar_keyframes = std::get<1>(it->second);
      auto start_value = std::get<1>(property_values_[it->first]);
      unsigned int start = scalar_keyframes.front().lastkeyTime;

      for (auto& keyframe : scalar_keyframes) {
        unsigned int bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
        glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
        glm::vec2 inPos(keyframe.inPos[0]);
        glm::vec2 outPos(keyframe.outPos[0]);
        BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start);
        auto curve = generator.getKeyframeCurve();//need to reduce start value
        start += curve.size();
      }
    }
  }

  //3.generate transform mat
  
}

//note: group's transform vector is 2D, and shape's transform vector is 3D
glm::vec3 Transform::GetShapeGrapOffset(){
  return std::get<t_Vector>(property_values_["Position"]) - std::get<t_Vector>(property_values_["Anchor Point"]);
}

void Transform::readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform) {
  if (propname != "Anchor Point" && propname != "Position" && propname != "Scale" && propname != "Rotation" && propname != "Opacity") return;
  bool keyvalue_is_vector = IsVectorProperty(propname);

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
      float lastkeyTime = static_cast<float>(it.value()["lastkeyTime"]) * frameRate;

      auto keyValue = it.value()["keyValue"];
      float keyTime = static_cast<float>(it.value()["keyTime"]) * frameRate;

      if (keyvalue_is_vector) {
        auto vector_lastkeyvalue = glm::vec3(lastkeyValue[0], lastkeyValue[1], lastkeyValue[2]);
        auto vector_keyvalue = glm::vec3(keyValue[0], keyValue[1], keyValue[2]);

        float out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"]) + lastkeyTime;
        auto out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"]) + vector_lastkeyvalue;

        float in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"]) + lastkeyTime;
        auto in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"]) + vector_lastkeyvalue;

        std::vector<glm::vec2> outpos_list, inpos_list;
        outpos_list.emplace_back(glm::vec2(out_x, out_y.x));
        outpos_list.emplace_back(glm::vec2(out_x, out_y.y));
        inpos_list.emplace_back(glm::vec2(in_x, in_y.x));
        inpos_list.emplace_back(glm::vec2(in_x, in_y.y));

        vector_keyframe.emplace_back(Keyframe<glm::vec3>(vector_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, vector_keyvalue, keyTime));
      }
      else {
        float scalar_lastkeyvalue = lastkeyValue;
        float scalar_keyvalue = keyValue;

        float out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
        auto out_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + scalar_lastkeyvalue;

        float in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
        auto in_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + scalar_lastkeyvalue;

        std::vector<glm::vec2> outpos_list, inpos_list;
        outpos_list.emplace_back(glm::vec2(out_x, out_y));
        inpos_list.emplace_back(glm::vec2(in_x, in_y));

        scalar_keyframe.emplace_back(Keyframe<float>(scalar_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, scalar_keyvalue, keyTime));
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

bool Transform::IsVectorProperty(std::string str) {
  return (str == "Anchor Point" || str == "Position" || str == "Scale");
}