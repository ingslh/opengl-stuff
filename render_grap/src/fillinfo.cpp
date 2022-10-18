#include "fillinfo.h"

FillInfo::FillInfo(const nlohmann::json& json){
  blend_mode = json["Blend Mode"];
  composite = json["Composite"];
  fill_rule = json["Fill Rule"];
  color.r = json["Color"][0];
  color.g = json["Color"][1];
  color.b = json["Color"][2];
  color.a = json["Color"][3];
  opacity = json["Opacity"];
}