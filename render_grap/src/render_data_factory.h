#pragma once
#include "layersinfo.h"
#include "pathinfo.h"
#include <mutex>
#include <glm/glm.hpp>

enum rDataType{
  tNONE_DATA = 0,
  tVERT_DATA,
  tCOLOR_DATA,
}; 

struct VertCluster{
  glm::vec2 start_pos;
  glm::vec2 out_dir;
  glm::vec2 in_dir;
  glm::vec2 end_pos;
};
typedef std::vector<VertCluster> SignalPathData;
typedef std::vector<SignalPathData> MultiPathsData;

class BaseRenderData{
public:
  rDataType GetType() const {
    return type_;
  }

  std::recursive_mutex& GetEditingLock(){
    return mutex_;
  }

private:
  rDataType type_;
  mutable std::recursive_mutex mutex_;
};

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
  unsigned int GetVertNumUsePathInd(unsigned int ind) const;
  unsigned int GetPathsCount() const {return paths_count_;}
  MultiPathsData& GetMultiPathsData(){return multi_paths_data_;}
  bool GetOutBezier() const{return out_bezier_;}
  std::vector<unsigned int> GetTriangleIndex(int ind) const {return bezier_vert_data_[ind].tri_index;}
  unsigned int GetTriangleIndexSize(unsigned int ind) const {return bezier_vert_data_[ind].tri_index.size();}
private:
  MultiPathsData multi_paths_data_;
  std::vector<BezierVertData> bezier_vert_data_;
  unsigned int paths_count_;
  bool out_bezier_ = true;
};

class ColorRenderData : public BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
};


typedef std::shared_ptr<BaseRenderData> BaseRenderDataPtr;
typedef std::shared_ptr<VerticesRenderData> VerticesRenderDataPtr;
typedef std::shared_ptr<ColorRenderData> ColorRenderDataPtr;

class RenderDataFactory{
public:
  RenderDataFactory(){};
  virtual ~RenderDataFactory(){};//auto call derived class destructor func

public:
  VerticesRenderData *CreateVerticesData(const LayersInfo* layer);
  ColorRenderData *CreateColorData(const LayersInfo* layer);
  bool ReleaseRenderData(BaseRenderData* data);
};

class SRenderDataFactory{
private:
  SRenderDataFactory(){
    render_data_factory_ = new RenderDataFactory();
  }

  ~SRenderDataFactory(){
    if(render_data_factory_)
      delete render_data_factory_;
  }

public:
  static SRenderDataFactory &GetIns(){
    static SRenderDataFactory kIns;
    return kIns;
  }

  VerticesRenderDataPtr CreateVerticesData(const LayersInfo* layer){
    return VerticesRenderDataPtr(render_data_factory_->CreateVerticesData(layer), [=](VerticesRenderData *data){
      render_data_factory_->ReleaseRenderData(data);
    });
  }

  ColorRenderDataPtr CreateColorData(const LayersInfo* layer){
    return ColorRenderDataPtr(render_data_factory_->CreateColorData(layer), [=](ColorRenderData *data){
      render_data_factory_->ReleaseRenderData(data);
    });
  }

private:
  RenderDataFactory* render_data_factory_;
};