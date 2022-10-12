#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

template<typename T>
struct vec3{
  T x;
  T y;
  T z;

  vec3 operator+(const vec3& a){
    vec3 ret;
    ret.x = this->x + a.x;
    ret.y = this->y + a.y;
    ret.z = this->z + a.z;
    return ret;
  }

  vec3 operator-(const vec3& a){
    vec3 ret;
    ret.x = this->x - a.x;
    ret.y = this->y - a.y;
    ret.z = this->z - a.z;
    return ret;
  }
};

class Transform{
public:
  Transform(const nlohmann::json& transform);
  vec3<float>& GetShapeGrapOffset();
  vec3<float>& GetPosition(){return position;}
private:
  vec3<float> anchor_point;
  vec3<float> position;
  vec3<unsigned int> scale;
  unsigned int rotation;
  unsigned int opacity;
};