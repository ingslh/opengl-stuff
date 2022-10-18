#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "transform.h"

enum EdgeRoundDir {
  t_UnKown = 0,
  t_Clockwise,
  t_AntiClockwise
};

struct Vertices {
  Vertices(glm::vec2 vert) : pos(vert), next(nullptr), previous(nullptr) {}
  Vertices() :next(nullptr), previous(nullptr) {}

  float cross(Vertices rhs) { return (this->pos.x * rhs.pos.y) - (this->pos.y * rhs.pos.y); }
  float dot(Vertices rhs) { return (this->pos.x * rhs.pos.x) + (this->pos.y * rhs.pos.y); }

  glm::vec2 pos;

  Vertices* next;
  Vertices* previous;

  bool operator==(const Vertices rhs) { return (this->pos.x == rhs.pos.x) && (this->pos.y == rhs.pos.y); }
  bool operator!=(const Vertices rhs) { return !((this->pos.x == rhs.pos.x) && (this->pos.y == rhs.pos.y)); }

  Vertices operator-(const Vertices rhs) { return Vertices(glm::vec2(this->pos.x - rhs.pos.x, this->pos.y - rhs.pos.y)); }
  Vertices operator+(const Vertices rhs) { return Vertices(glm::vec2(this->pos.x + rhs.pos.x, this->pos.y + rhs.pos.y)); }
};

class Polygon{
public:
  Polygon(const std::vector<glm::vec2>& vertices);
  Polygon(Polygon* p_Parent, glm::vec2 a, glm::vec2 b, glm::vec2 c);
  Polygon(Polygon* p_Parnet, glm::vec2 a, glm::vec2 b);
  Polygon(Polygon* p_Parent, glm::vec2 a);
  Polygon(Polygon* p_Parent);
  Polygon();
  ~Polygon();

  bool addChild(Polygon *child);

  //Return true if point was added. False if adding failed (point already exists in Polygon)
  bool addVert(glm::vec2 vert);

  //Return true if point was removed. False if point DNE in Polygon (including if Polygon is empty)
  bool removeVert(glm::vec2 vert);
  bool removeVert(unsigned int ind);
  bool removeVert(Vertices& vert);

  Vertices* getHead() { return head; }
  Polygon* getChild(unsigned int ind) { return (children.size() > ind) ? children[ind] : nullptr; }
  bool removeChild(unsigned int ind);
  std::vector<Polygon*> getChildren() { return children; }

  //Find a specific point
  Vertices* getPoint(float x, float y);
  Vertices* getPoint(glm::vec2 point);
  Vertices* getPoint(unsigned position);

  inline unsigned numPoints() { return vert_nums_; }
  inline unsigned numChildren() { return children.size(); }

  /**
  * Creates a new Point at location of p_Point->previous. Returns TRUE if Point already exists in Polygon.<br>
  * Insert allows for multiple copies of the same data point, unlike addPoint
  */
  bool insertPoint(glm::vec2 a, Vertices* p_Point);
  void reverse(int ind);

  //void SplitToConvex();
  bool IsConvex()const { return is_convex_; }

protected:
  Vertices* head;
  std::vector<Polygon*> children;
  unsigned int vert_nums_;

private:
  bool is_convex_;
  EdgeRoundDir edge_vec_dir_;
  std::vector<glm::vec2> edges_;
  std::vector<glm::vec2> ret_verts_;

  Polygon* parent_ = nullptr;
};