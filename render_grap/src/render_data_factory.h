#pragma once
#include "layersinfo.h"
#include "pathinfo.h"
#include "vertices_render_data.hpp"
#include "color_render_data.hpp"

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