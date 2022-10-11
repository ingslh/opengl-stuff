#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "layersinfo.h"

class JsonReader{
public:
  JsonReader(const std::string& path);
  int getLayersCount();
  std::shared_ptr<LayersInfo> GetLayersInfo(unsigned int ind);

private:
  nlohmann::json root_;
  int layers_count_;
  std::vector<std::shared_ptr<LayersInfo>> layers_;
};