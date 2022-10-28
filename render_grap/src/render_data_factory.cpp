#include "render_data_factory.h"
#include <mutex>

template<typename T>
void SafeDelete(BaseRenderData* data){
  auto concreate = static_cast<T*>(data);
  {
    std::lock_guard<std::recursive_mutex> guard(concreate->BaseRenderData::GetEditingLock());
    delete data;
  }
}

VerticesRenderData* RenderDataFactory::CreateVerticesData(const LayersInfo* layer){
  return new VerticesRenderData(layer);
}

ColorRenderData* RenderDataFactory::CreateColorData(const LayersInfo* layer){
  return new ColorRenderData(layer);
}

TransformRenderData* RenderDataFactory::CreateTransformData(const LayersInfo* layer){
  return new TransformRenderData(layer);
}

bool RenderDataFactory::ReleaseRenderData(BaseRenderData* data){
  if(!data) return false;

  switch (data->GetType())
  {
  case rDataType::tVerticesData:
    SafeDelete<VerticesRenderData>(data);
    break;
  case rDataType::tColorData:
    SafeDelete<ColorRenderData>(data);
  case rDataType::tTransformData:
    SafeDelete<TransformRenderData>(data);
  default:
    return false;
  }
  return true;
}