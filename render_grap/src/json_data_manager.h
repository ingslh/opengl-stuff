#pragma once
#include <nlohmann/json.hpp>

struct LayerBaseInfo {
  unsigned int index;
  float in_pos;
  float out_pos;
};

class JsonDataManager{
public:
  static JsonDataManager& GetIns();
  void SetBasicInfo(nlohmann::json& json);
  unsigned int GetWidth(){return width_;}
  unsigned int GetHeight(){return height_;}
  unsigned int GetFrameRate(){return frame_rate_;}
  unsigned int GetDuration(){return duration_;}
  unsigned int GetLayersNum(){return layers_num_;}
  void AddLayerBaseInfo(unsigned int ind, float in_pos, float out_pos);

private:
  unsigned int width_;
  unsigned int height_;
  unsigned int frame_rate_;
  unsigned int duration_;
  unsigned int layers_num_;
  std::vector<LayerBaseInfo> layers_base_info_;
};