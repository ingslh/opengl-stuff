#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>

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
  glm::vec3 GetShapeGrapOffset();
  glm::vec3& GetPosition(){return position;}
private:
  glm::vec3 anchor_point;
  glm::vec3 position;
  glm::uvec3 scale;
  unsigned int rotation;
  unsigned int opacity;
};