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

class VerticesRenderData : public BaseRenderData{
public:
  VerticesRenderData(const LayersInfo* layer);
  MultiPathsData& GetMultiPathsData(){return multi_paths_data_;}
  glm::vec2 NormalizeVec2(const glm::vec2& pos);
  bool ConverToOpenglVert(unsigned int ind_path, unsigned int ind_vert, std::vector<float>& vert_info);
  bool ConverToOpenglVert(unsigned int ind_path, std::vector<float>& vert_info);
  unsigned int GetVertNumUsePathInd(unsigned int ind) const { return static_cast<unsigned int>(multi_paths_data_[ind].size()); };
private:
  MultiPathsData multi_paths_data_;
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