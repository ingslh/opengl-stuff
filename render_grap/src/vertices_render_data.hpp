#pragma once
#include "base_render_data.hpp"
#include "layersinfo.h"
#include "pathinfo.h"
#include <glm/glm.hpp>
#include <vector>

struct VertCluster{
  glm::vec2 start_pos;
  glm::vec2 out_dir;
  glm::vec2 in_dir;
  glm::vec2 end_pos;
};
typedef std::vector<VertCluster> SignalPathData;
typedef std::vector<SignalPathData> MultiPathsData;

struct BezierVertData{
  std::vector<float> verts;//3-dim
  std::vector<unsigned int> tri_index;//3 int make up a triangle
};

class VerticesRenderData : public BaseRenderData{
public:
  VerticesRenderData(const LayersInfo* layer, bool out_bezier = true);
  glm::vec2 NormalizeVec2(const glm::vec2& pos);
  glm::vec3 NormalizeVec3(const glm::vec3& pos);
  bool ConverToOpenglVert(unsigned int ind_path, std::vector<float>& vert_info);
  unsigned int GetVertNumByPathInd(unsigned int ind) const;
  unsigned int GetPathsCount() const {return paths_count_;}
  MultiPathsData& GetMultiPathsData(){return multi_paths_data_;}
  bool GetOutBezier() const{return out_bezier_;}
  std::vector<unsigned int> GetTriangleIndex(int ind) const {return bezier_vert_data_[ind].tri_index;}
  unsigned int GetTriangleIndexSize(unsigned int ind) const {return static_cast<unsigned int>(bezier_vert_data_[ind].tri_index.size());}
private:
  MultiPathsData multi_paths_data_;
  std::vector<BezierVertData> bezier_vert_data_;
  unsigned int paths_count_;
  bool out_bezier_ = true;
};

typedef std::shared_ptr<VerticesRenderData> VerticesRenderDataPtr;