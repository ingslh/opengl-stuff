#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <variant>

enum TransformProp {
  k_AnchorPos = 0,
  k_Position,
  k_Scale,
  k_Rotation,
  k_Opacity,
};

enum PropertyOrKeyvalueType {
  t_Vector = 0,
  t_Scalar,
};

template<typename T>
struct Keyframe{
  T keyValue;
  float keyTime;
  std::vector<float> bezier_dirs;//influ, outinflu, inspeed, outspeed

  Keyframe(T keyValue_, float keyTime_, std::vector<float> bezier_dirs_) :
    keyValue(keyValue_), keyTime(keyTime_), bezier_dirs(bezier_dirs_){}
};

typedef std::vector<Keyframe<glm::vec3>> VectorKeyFrames;
typedef std::vector<Keyframe<float>> ScalarKeyFrames;

class Transform{
public:
  Transform(const nlohmann::json& transform);
  glm::vec3 GetShapeGrapOffset();
  glm::vec3& GetPosition(){return std::get<t_Vector>(property_values_["Position"]);}

protected:
  void readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform);
private:
  std::map<std::string, std::variant<glm::vec3, float>> property_values_;
  std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> keyframe_data_;
};