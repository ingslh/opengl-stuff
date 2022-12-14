#include "transform.h"
#include "json_data_manager.h"
#include "bezier_generator.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(const nlohmann::json& transform, bool IsShapeTransform){

  //1.read layer's transofrm infos
  for (auto& el : transform.items()) {
    readKeyframeandProperty(el.key(),el.value());
  }
  type_ = IsShapeTransform ? t_ShapeTrans : t_GroupTrans;
  if (!IsShapeTransform) return;

  //2.generate bezier curve data
  for (auto it = keyframe_data_.begin(); it != keyframe_data_.end(); it++) {
    if (IsVectorProperty(it->first)) {
      auto vector_keyframes = std::get<0>(it->second);
      //auto start_value = it->first == "Scale" ? glm::vec2(100,100): vector_keyframes[0].lastkeyValue;
      auto start_value =  vector_keyframes[0].lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::vector<std::map<unsigned int, float>> double_curve_line;
      double_curve_line.resize(2);

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
        BezierGenerator generator_x(lastPos_x, outPos_x, inPos_x, curPos_x, bezier_duration, static_cast<unsigned int>(start), start_value.x);
        auto curve_x = generator_x.getKeyframeCurveMap();
        BezierGenerator generator_y(lastPos_y, outPos_y, inPos_y, curPos_y, bezier_duration, static_cast<unsigned int>(start), start_value.y);
        auto curve_y = generator_y.getKeyframeCurveMap();
        start += static_cast<unsigned int>(curve_x.size()); 
        double_curve_line[0].merge(curve_x);
        double_curve_line[1].merge(curve_y);
      }
      transform_curve_[it->first] = double_curve_line;
    }
    else {
      auto scalar_keyframes = std::get<1>(it->second);
      auto start_value = it->first == "Rotation" ? 0.0 : scalar_keyframes[0].lastkeyValue;
      //auto start_value = scalar_keyframes[0].lastkeyValue;
      unsigned int start = static_cast<unsigned int>(scalar_keyframes.front().lastkeyTime);
      std::vector<std::map<unsigned int, float>> signal_curve_line;

      signal_curve_line.resize(1);

      for (auto& keyframe : scalar_keyframes) {
        unsigned int bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
        glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
        glm::vec2 inPos(keyframe.inPos[0]);
        glm::vec2 outPos(keyframe.outPos[0]);
        BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start, start_value);
        auto curve = generator.getKeyframeCurveMap();
        start += static_cast<unsigned int>(curve.size());

        signal_curve_line[0].merge(curve);//cpp17 support,if old cpp verison can use "signal_curve_line[0].insert(curve.begin(),curve.end());" 
      }
      transform_curve_[it->first] = signal_curve_line;
    }
  }
}

//note: group's transform vector is 2D, and shape's transform vector is 3D
glm::vec3 Transform::GetShapeGrapOffset(){
  return std::get<t_Vector>(property_values_["Position"]) - std::get<t_Vector>(property_values_["Anchor Point"]);
}

//note:not support anchor position's keyframe,can conver to position's keyframe
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
      property_values_[propname] = glm::vec3(cur_property["Curve1"]["lastkeyValue"][0], cur_property["Curve1"]["lastkeyValue"][1], cur_property["Curve1"]["lastkeyValue"][2]);
    else
      property_values_[propname] = cur_property["Curve1"]["lastkeyValue"];
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

        float out_x, in_x;
        glm::vec3 out_y, in_y;
        if (it.value()["OutPos"]["x"].is_number() && it.value()["OutPos"]["y"].is_number()) {
          out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"]) + lastkeyTime;
          out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"]) + vector_lastkeyvalue;
          in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"]) + lastkeyTime;
          in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"]) + vector_lastkeyvalue;
        }
        else {
          out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
          out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + vector_lastkeyvalue;
          in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
          in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + vector_lastkeyvalue;
        }

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

void Transform::GenerateTransformMat() {
  auto reslution = glm::vec3(JsonDataManager::GetIns().GetWidth(), JsonDataManager::GetIns().GetHeight(), 0);
  auto position = std::get<t_Vector>(property_values_["Position"]) / reslution - glm::vec3(0.5,0.5,0);
  auto frame_lenth = transform_mat_.clip_end - transform_mat_.clip_start + 1;
  for (unsigned int i = 0; i < frame_lenth; i++) {
    glm::mat4 trans = glm::mat4(1.0f);
    if (transform_curve_.count("Position")) {
      std::vector<float> offset;
      offset.resize(2);
      for (auto j = 0; j < offset.size(); j++) {
        if(i < transform_curve_["Position"][j].begin()->first)
          offset[j] = transform_curve_["Position"][j].begin()->second;
        else if( i > transform_curve_["Position"][j].rbegin()->first)
          offset[j] = transform_curve_["Position"][j].rbegin()->second;
        else
          offset[j] = transform_curve_["Position"][j][i];
      }
      trans = glm::translate(trans, glm::vec3(offset.front()/reslution.x, offset.back()/reslution.y, 0));
    }

    if (transform_curve_.count("Rotation")) {
      float rot;
      if (i < transform_curve_["Rotation"].front().begin()->first)
        rot = transform_curve_["Rotation"].front().begin()->second;
      else if(i > transform_curve_["Rotation"].front().rbegin()->first)
        rot = transform_curve_["Rotation"].front().rbegin()->second;
      else
        rot = transform_curve_["Rotation"].front()[i];
      auto t1 = glm::translate(glm::mat4(1), -glm::vec3(position.x, position.y, 0));
      auto r = glm::rotate(glm::mat4(1), glm::radians(-rot), glm::vec3(0, 0, 1.0));//ae use left-hand CS ,but opengl us right-hand CS(coordinate system)
      auto t2 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, 0));
      trans = trans * t2 * r * t1;
    }

    if (transform_curve_.count("Scale")) {
      std::vector<float> scale;
      scale.resize(2);
      for (auto j = 0; j < scale.size(); j++) {
        if (i < transform_curve_["Scale"][j].begin()->first)
          scale[j] = transform_curve_["Scale"][j].begin()->second;
        else if (i > transform_curve_["Scale"][j].rbegin()->first)
          scale[j] = transform_curve_["Scale"][j].rbegin()->second;
        else
          scale[j] = transform_curve_["Scale"][j][i];
      }
      trans = glm::scale(trans, glm::vec3(scale.front() / 100, scale.back() / 100, 1.0));
    }
    transform_mat_.trans.emplace_back(trans);
  }
}

bool Transform::IsVectorProperty(std::string str) {
  return (str == "Anchor Point" || str == "Position" || str == "Scale");
}

void Transform::SetInandOutPos(unsigned int ind, float in_pos, float out_pos) {
  transform_mat_.layer_index = ind;
  auto frameRate = JsonDataManager::GetIns().GetFrameRate();
  auto duration = JsonDataManager::GetIns().GetDuration();
  transform_mat_.clip_start = in_pos * frameRate < 0 ? 0 : in_pos * frameRate;
  transform_mat_.clip_end = out_pos > duration  ? duration * frameRate : out_pos * frameRate;
}