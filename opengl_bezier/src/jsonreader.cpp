#include "jsonreader.h"
#include <fstream>
#include <iostream>

JsonReader::JsonReader(const std::string& path){
  std::ifstream json_stream;
  json_stream.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try{
    json_stream.open(path);
    root_ = nlohmann::json::parse(json_stream);
    json_stream.close();
  }
  catch (std::ifstream::failure& e){
    std::cout << "ERROR::JSON::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
  }

  if (!root_.is_null()) {
    auto layers = root_[root_.begin().key()]["layers"];
    for (auto& el_c : layers.items()) {
      auto cotents = el_c.value()["Contents"];
      for (auto& el_g : cotents.items()) {
        auto group_contents = el_g.value()["Contents"];
        auto group_transform = el_g.value()["Transform"];
      }
    }
  }
}

int JsonReader::getLayersCount() {
  if (layers_count_ != -1)
    return layers_count_;
  else if(!root_.is_null()){
    return root_[root_.begin().key()]["layers"].size();
  }
  else {
    return -1;
  }
}