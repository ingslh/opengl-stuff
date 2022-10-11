#include "pathinfo.h"

PathInfo::PathInfo(const nlohmann::json& json){
  shape_direction = json["Shape Direction"];
  closed = json["Closed"];
  auto outpos_list = json["OutPos"];
  auto inpos_list = json["InPos"];
  auto vert_list = json["Vertices"];
  for(auto ind = 0; ind < outpos_list.size(); ind++){
    vec2 outpos;
    outpos.x = outpos_list[ind][0];
    outpos.y = outpos_list[ind][1];
    out_pos_vec.emplace_back(outpos);

    vec2 inpos;
    inpos.x = inpos_list[ind][0];
    inpos.y = inpos_list[ind][1];
    in_pos_vec.emplace_back(inpos);

    vec2 vert;
    vert.x = vert_list[ind][0];
    vert.y = vert_list[ind][1];
    vertices.emplace_back(vert);
  }
}