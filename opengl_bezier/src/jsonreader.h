#pragma once
#include <nlohmann/json.hpp>
#include <string>

class JsonReader{
public:
  JsonReader(const std::string& path);
  int getLayersCount();

private:
  nlohmann::json root_;
  int layers_count_;
};