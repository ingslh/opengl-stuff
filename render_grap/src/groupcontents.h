#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include "pathinfo.h"
#include "fillinfo.h"

class PathInfo;
class FillInfo;
class GroupContents{
public:
  GroupContents(const nlohmann::json& json);
  std::vector<std::shared_ptr<PathInfo>>& GetPaths(){return paths;}

private:
  std::vector<std::shared_ptr<PathInfo>> paths;
  std::vector<std::shared_ptr<FillInfo>> fills;
};