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
  T lastkeyValue;
  float lastkeyTime;
  glm::vec2 outPos;
  glm::vec2 inPos;
  T keyValue;
  float keyTime;

  Keyframe(T lastkeyValue_, float lastkeyTime_, glm::vec2 outPos_, glm::vec2 inPos_, T keyValue_, float keyTime_) :
    lastkeyValue(lastkeyValue_), lastkeyTime(lastkeyTime_), outPos(outPos_), inPos(inPos_), keyValue(keyValue_), keyTime(keyTime_){}
};

typedef std::vector<Keyframe<glm::vec3>> VectorKeyFrames;
typedef std::vector<Keyframe<float>> ScalarKeyFrames;
typedef std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframesMap;

class Transform{
public:
  Transform(const nlohmann::json& transform);
  glm::vec3 GetShapeGrapOffset();
  glm::vec3& GetPosition(){return std::get<t_Vector>(property_values_["Position"]);}
  KeyframesMap& GetKeyframeData(){return keyframe_data_;}

protected:
  void readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform);
private:
  std::map<std::string, std::variant<glm::vec3, float>> property_values_;
  KeyframesMap keyframe_data_;
};