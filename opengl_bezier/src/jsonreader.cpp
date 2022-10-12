#include "jsonreader.h"
#include "json_data_manager.h"
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
  JsonDataManager::GetIns().SetBasicInfo(root_[root_.begin().key()]);

  if (!root_.is_null()) {
    //get layers info
    auto layers = root_[root_.begin().key()]["layers"];
    for (auto& el_c : layers.items()) {
      layers_.emplace_back(std::make_shared<LayersInfo>(el_c.value()));
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

std::shared_ptr<LayersInfo> JsonReader::GetLayersInfo(unsigned int ind){
  if(ind < layers_.size() && layers_.size())
    return layers_[ind];
  else
    return std::shared_ptr<LayersInfo>(NULL);
}