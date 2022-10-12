#pragma once
#include "layersinfo.h"
#include "pathinfo.h"

enum rDataType{
  tNONE_DATA = 0,
  tVERT_DATA,
  tCOLOR_DATA,
}; 

struct VertCluster{
  vec2 start_pos;
  vec2 out_dir;
  vec2 in_dir;
  vec2 end_pos;
};
typedef std::vector<VertCluster> SignalPathData;
typedef std::vector<SignalPathData> MultiPathsData;

class BaseRenderData{
public:
  rDataType GetType() const {
    return type_;
  }

private:
  rDataType type_;
};

class VerticesRenderData : public BaseRenderData{
public:
  VerticesRenderData(const LayersInfo* layer);
  MultiPathsData& GetMultiPathsData(){return multi_paths_data_;}
  vec2 NormalizeVec2(const vec2& pos);
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