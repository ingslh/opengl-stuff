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

bool RenderDataFactory::ReleaseRenderData(BaseRenderData* data){
  if(!data) return false;

  switch (data->GetType())
  {
  case rDataType::tVERT_DATA:
    SafeDelete<VerticesRenderData>(data);
    break;
  case rDataType::tCOLOR_DATA:
    SafeDelete<ColorRenderData>(data);
  default:
    return false;
  }
  return true;
}