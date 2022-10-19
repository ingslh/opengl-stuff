#include "pathinfo.h"
#include "polygon.h"
#include "earcut.hpp"
#include "bezier_generator.hpp"

PathInfo::PathInfo(const nlohmann::json& json){
  shape_direction = json["Shape Direction"];
  closed = json["Closed"];
  auto outpos_list = json["OutPos"];
  auto inpos_list = json["InPos"];
  auto vert_list = json["Vertices"];
  for(auto ind = 0; ind < outpos_list.size(); ind++){
    glm::vec2 outpos;
    outpos.x = outpos_list[ind][0];
    outpos.y = outpos_list[ind][1];
    out_pos_vec.emplace_back(outpos);

    glm::vec2 inpos;
    inpos.x = inpos_list[ind][0];
    inpos.y = inpos_list[ind][1];
    in_pos_vec.emplace_back(inpos);

    glm::vec2 vert;
    vert.x = vert_list[ind][0];
    vert.y = vert_list[ind][1];
    vertices.emplace_back(vert);
  }

  if(closed){
    auto bezier_verts = std::make_shared<BezierGenerator>(vertices, out_pos_vec, in_pos_vec);
    //auto bezier_test = bezier_verts -> getBezierVerts();
    auto polygon = std::make_shared<PolygonArray>(bezier_verts->getBezierVerts());
    //auto test = polygon->getVertices();
    auto indices = mapbox::earcut<int>(polygon->getVertices());
  }
}