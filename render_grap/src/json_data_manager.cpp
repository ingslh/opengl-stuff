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