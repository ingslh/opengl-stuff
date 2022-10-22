#pragma once
#include <mutex>

enum rDataType{
  tNONE_DATA = 0,
  tVERT_DATA,
  tCOLOR_DATA,
}; 

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

typedef std::shared_ptr<BaseRenderData> BaseRenderDataPtr;