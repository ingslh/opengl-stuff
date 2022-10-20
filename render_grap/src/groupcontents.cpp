#include "groupcontents.h"

GroupContents::GroupContents(const nlohmann::json& json){
  for(auto& el : json.items()){
    if(el.key().substr(0,4) == "Path"){
      paths.emplace_back(std::make_shared<PathInfo>(json[el.key()]));
    }
    else if(el.key().substr(0,4) == "Fill"){
      fills.emplace_back(std::make_shared<FillInfo>(json[el.key()]));
    }
    else if (el.key().substr(0, 11) == "Merge Paths") { 
      auto merge_mode = json[el.key()]["Mode"];
    }
  }

  if (paths.size() > 1) { //exist multi path

  }
}