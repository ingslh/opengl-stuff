#include "json_data_manager.h"

JsonDataManager& JsonDataManager::GetIns(){
  static JsonDataManager kManager;
  return kManager;
}

void JsonDataManager::SetBasicInfo(nlohmann::json& json){
  width_ = json["width"];
  height_ = json["height"];
  frame_rate_ = json["frameRate"];
  duration_ = json["duration"];
  layers_num_ = json["numLayers"];
}

void JsonDataManager::AddLayerBaseInfo(unsigned int ind, float in_pos, float out_pos) {
  LayerBaseInfo tmp;
  tmp.index = ind;
  tmp.in_pos = in_pos;
  tmp.out_pos = out_pos;
  layers_base_info_.emplace_back(tmp);
}