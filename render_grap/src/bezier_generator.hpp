#pragma once
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

struct BezierCluster{
  vec2 start;
  vec2 out;
  vec2 in;
  vec2 end;
};

class BezierGenerator{
public:
  BezierGenerator(const std::vector<vec2>& vert, const std::vector<vec2>& out, const std::vector<vec2>& in){
    auto vert_nums = vert.size();
    std::vector<BezierCluster> cluster_list;
    for(size_t i = 0; i < vert_nums; i++){
      BezierCluster cluster;
      cluster.start = vert[i];
      cluster.out = out[i] + cluster.start;

      if(i == vert_nums -1){
        cluster.end = vert[0];
        cluster.in = in[0] + cluster.end;
      }
      else{
        cluster.end = vert[i + 1];
        cluster.in = in[i + 1] + cluster.end;
      }
      cluster_list.emplace_back(cluster);
    }

    for(auto& it : cluster_list){
      if(it.in.x == 0 && it.in.y == 0 && it.out.x ==0 && it.out.y == 0){
        bezier_verts_.emplace_back(it.start);
      }
      else{ //need to generate bezier
        auto delta = 1.0/float(segments_);
        for(unsigned int i = 0; i < segments_; i++){
          float t = static_cast<float>(delta * float(i));
          vec2 p;
          p.x = (1 - t) * (1 - t) * (1 - t) * it.start.x + 3 * t * (1 - t) * (1 - t)* it.out.x + 3 * t*t* (1 - t)* it.in.x + t * t * t * it.end.x;
          p.y = (1 - t) * (1 - t) * (1 - t) * it.start.y + 3 * t * (1 - t) * (1 - t)* it.out.y + 3 * t*t* (1 - t)* it.in.y + t * t * t * it.end.y;
          bezier_verts_.emplace_back(p);
        }
      }
    }
  }

  std::vector<vec2> getBezierVerts() const {return bezier_verts_;}

private:
  std::vector<vec2> bezier_verts_;
  unsigned int segments_ = 10;
};