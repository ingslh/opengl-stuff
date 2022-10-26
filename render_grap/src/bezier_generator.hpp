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
        auto delta = 1.0/float(default_segments_);
        for(unsigned int i = 0; i < default_segments_; i++){
          float t = static_cast<float>(delta * float(i));
          vec2 p;
          p.x = (1 - t) * (1 - t) * (1 - t) * it.start.x + 3 * t * (1 - t) * (1 - t)* it.out.x + 3 * t*t* (1 - t)* it.in.x + t * t * t * it.end.x;
          p.y = (1 - t) * (1 - t) * (1 - t) * it.start.y + 3 * t * (1 - t) * (1 - t)* it.out.y + 3 * t*t* (1 - t)* it.in.y + t * t * t * it.end.y;
          bezier_verts_.emplace_back(p);
        }
      }
    }
  }

  BezierGenerator(const vec2& lastPos, const vec2& outPos, const vec2& inPos, const vec2& curPos, unsigned int segments, unsigned int start){
    for (unsigned int i = start; i <= segments + start; i++) {
      std::vector<float> ret;
      std::vector<float> elements;
      elements.emplace_back(curPos.x - 3 * inPos.x + 3 * outPos.x - lastPos.x);
      elements.emplace_back(3 * inPos.x - 6 * outPos.x + 3 * lastPos.x);
      elements.emplace_back(3 * outPos.x - 3 * lastPos.x);
      elements.emplace_back(lastPos.x - i);
      CubicPolynomial(0, 1, elements, ret);
      if (ret.size() == 1) {
        auto t = ret[0];
        auto p = (1 - t) * (1 - t) * (1 - t) * lastPos.y + 3 * t * (1 - t) * (1 - t)* outPos.y + 3 * t*t* (1 - t)* inPos.y + t * t * t * curPos.y;
        keyframe_curve_.emplace_back(glm::vec2(i, p));
      }
    }
  }


  std::vector<vec2> getBezierVerts() const {return bezier_verts_;}
  std::vector<vec2> getKeyframeCurve() const {return keyframe_curve_;}

private:
  void CubicPolynomial(float l, float r, const std::vector<float>& elements, std::vector<float>& ret) {
    auto a = elements[0], b = elements[1], c = elements[2], d = elements[3];
    auto f = [a, b, c, d](float x)->float {
      return ((a*x + b)*x + c)*x + d;
    };
    if (f(l)*f(r) > 0 && ((r - 1) < 1)) return;
    float mid = (l + r) / 2;
    if (f(mid) <= 1e-4 && f(mid) >= -1e-4) {
      ret.emplace_back(round(mid * pow(10,3)) * pow(10,-3));
      return;
    }
    CubicPolynomial(l, mid, elements, ret), CubicPolynomial(mid, r, elements, ret);
  }

  std::vector<vec2> bezier_verts_;
  std::vector<vec2> keyframe_curve_;
  unsigned int default_segments_ = 10;
};