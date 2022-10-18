#pragma once
#include <nlohmann/json.hpp>
#include <vector>

struct Color4V{
  float r;
  float g;
  float b;
  unsigned int a;
};

class FillInfo{
public:
  FillInfo(const nlohmann::json& json);

private:
  unsigned int blend_mode;
  unsigned int composite;
  unsigned int fill_rule;
  Color4V color;
  unsigned int opacity;
};