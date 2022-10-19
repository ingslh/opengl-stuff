#include "polygon.h"


Polygon::Polygon(const std::vector<glm::vec2>& vert, ContainerType type) :edge_vec_dir_(EdgeRoundDir::t_eClockwise) {
  auto size = vert.size();
  vert_nums_ = size;
  if(type == ContainerType::t_cLinkedList){
    head = new Vertices(vert[0]);
    auto old = head;
    for (auto ind = 1; ind < size; ind++) {
      auto in_vert = new Vertices(vert[ind]);
      old->next = in_vert;
      in_vert->previous = old;
      if (ind == size - 1) {
        in_vert->next = head;
        head->previous = in_vert;
      }
      old = in_vert;
    }
  }
  else if(type == ContainerType::t_cVector){
    vertices_.push_back(vert);
  }
}

Polygon::Polygon(Polygon* p_Parent, glm::vec2 cur, glm::vec2 next, glm::vec2 pre){
  parent_ = p_Parent;
  Vertices* a = new Vertices(cur);
  Vertices* b = new Vertices(next);
  Vertices* c = new Vertices(pre);

  head = a;
  head->next = b;
  head->previous = c;

  b->next = c;
  b->previous = a;

  c->next = a;
  c->previous = b;

  vert_nums_ = 3;

  if (parent_) parent_->addChild(this);
}

Polygon::Polygon(Polygon* p_Parent, glm::vec2 cur, glm::vec2 next){
  parent_ = p_Parent;
  Vertices* a = new Vertices(cur);
  Vertices* b = new Vertices(next);

  head = a;
  head->next = b;
  head->previous = b;

  b->next = a;
  b->previous = a;

  vert_nums_ = 2;

  if (parent_) parent_->addChild(this);
}

Polygon::Polygon(Polygon *p_Parent, glm::vec2 cur){
  parent_ = p_Parent;
  Vertices* a = new Vertices(cur);

  head = a;
  head->next = a;
  head->previous = a;

  vert_nums_ = 1;

  if (parent_) parent_->addChild(this);
}

Polygon::Polygon(Polygon *p_Parent){
  parent_ = p_Parent;
  head = nullptr;
  vert_nums_ = 0;

  if (parent_) parent_->addChild(this);
}

Polygon::Polygon(){
  parent_ = nullptr;
  head = nullptr;
  vert_nums_ = 0;
}

Polygon::~Polygon(){
  Vertices* deleteMe;

  while (vert_nums_ != 0){
    deleteMe = head->previous;
    head->previous = head->previous->previous;
    delete deleteMe;
    vert_nums_--;
  }
}

bool Polygon::addChild(Polygon *child){
  if (!child) return false;

  children.push_back(child);
  return true;
}

bool Polygon::removeChild(unsigned int pos)
{
  if (pos > children.size()) return false;

  delete children[pos];
  children.erase(children.begin() + pos);

  return true;
}

bool Polygon::addVert(glm::vec2 vert) {
  if (!head) {
    head = new Vertices(vert);
    head->next = head;
    head->previous = head;
    vert_nums_++;
    return true;
  }

  Vertices* find = head;
  do{
    find = find->next;
    if (find == head) break;
  } while (find->pos.x != vert.x && find->pos.y != vert.y);

  // If point is not already in the Polygon
  if ((find->pos.x != vert.x) || (find->pos.y != vert.y))
  {
    find = new Vertices(vert);

    find->next = head;
    find->previous = head->previous;
    find->previous->next = find;
    head->previous = find;

    vert_nums_++;
    return true;
  }
  return false;
}

bool Polygon::removeVert(glm::vec2 vert) {
  if (vert_nums_ == 0) return false;

  Vertices* find = head;
  do{
    find = find->next;

    if (find == head) break;
  } while (find->pos.x != vert.x && find->pos.y != vert.y);

  if (find->pos.x == vert.x && find->pos.y == vert.y){
    find->previous->next = find->next;
    find->next->previous = find->previous;

    if (head == find) head = find->next;

    delete(find);
    vert_nums_--;
    if (vert_nums_ == 0)
      head = NULL;
    return true;
  }
  return false;
}

bool Polygon::removeVert(unsigned int ind) {
  if (ind > vert_nums_ || vert_nums_ == 0) return false;
  Vertices* find = head;

  for (int i = 0; i < ind; i++)
    find = find->next;

  find->previous->next = find->next;
  find->next->previous = find->previous;

  if (head == find) head = find->next;
  delete find;
  vert_nums_--;
  if (vert_nums_ == 0)
    head = nullptr;
  return true;
}

bool Polygon::removeVert(Vertices& vert) {
  if (!vert_nums_) return false;
  Vertices* find = head;
  do {
    if (find->pos.x == vert.pos.x && find->pos.y == vert.pos.y &&
      find->next->pos.x == vert.next->pos.x && find->next->pos.y == vert.next->pos.y &&
      find->previous->pos.x == vert.previous->pos.x && find->previous->pos.y == vert.previous->pos.y)
      break;
    find = find->next;
  } while (find != head);
  if (find->pos.x == vert.pos.x && find->pos.y == vert.pos.y) {
    find->previous->next = find->next;
    find->next->previous = find->previous;

    if (head == find) head = find->next;
    delete(find);
    vert_nums_--;
    if (vert_nums_ == 0) head = nullptr;
    return true;
  }
  return false;
}

Vertices* Polygon::getPoint(float x, float y)
{
  Vertices* find = head;

  do{
    if (find->pos.x == x && find->pos.y == y)
      return find;

    find = find->next;
  } while (find != head);

  return nullptr;
}

Vertices* Polygon::getPoint(unsigned pos)
{
  Vertices* find = head;

  if (pos >= vert_nums_) return nullptr;
  for (int i = 0; i != pos; i++)
    find = find->next;

  return find;
}

Vertices* Polygon::getPoint(glm::vec2 vert) {
  Vertices* find = head;
  do {
    if (find->pos.x == vert.x && find->pos.y == vert.y)
      return find;

    find = find->next;
  } while (find != head);

  return nullptr;
}

bool Polygon::insertPoint(glm::vec2 vert, Vertices* p_Point) {
  if (!p_Point) return false;
  Vertices* find = head;
  for (auto i = 0; i < vert_nums_; i++) {
    if (find->pos.x == p_Point->pos.x && find->pos.y == p_Point->pos.y)
      break;
    find = find -> next;
  }

  if (find->pos.x != p_Point->pos.x && find->pos.y != p_Point->pos.y) return false;

  Vertices* newPoint = new Vertices(vert);

  find->previous->next = newPoint;
  newPoint->previous = find->previous;

  find->previous = newPoint;
  newPoint->next = find;

  vert_nums_++;

  return true;
}

void Polygon::reverse(int pos) {
  if (pos >= 0) {
    if (pos < children.size())
      children[pos]->reverse(-1);
    return;
  }

  // pos == -1; so reverse this polygon
  Vertices* active = head;
  Vertices* temp;

  do{
    temp = active->next;

    active->next = active->previous;
    active->previous = temp;

    active = temp;
  } while (active != head);
}

typedef Vertices Vector;
bool Polygon::isConvex(Vertices* active){
  auto pre = *active->previous;
  auto cur = *active;
  auto next = *active->next;

  Vector vec1 = cur - pre;
  Vector vec2 = next - cur;
  auto cross_ret = glm::cross(glm::vec3(vec1.pos.x, vec1.pos.y, 0), glm::vec3(vec2.pos.x, vec2.pos.y, 0));
  return cross_ret.z > 0;
}

typedef Vertices Vector;
bool Polygon::inTriangle(Vertices pointToCheck, Vertices earTip, Vertices earTipPlus, Vertices earTipMinus){
  if( ( pointToCheck.pos.x == earTip.pos.x && pointToCheck.pos.y == earTip.pos.y ) ||
			( pointToCheck.pos.x == earTipPlus.pos.x && pointToCheck.pos.y == earTipPlus.pos.y ) ||
			( pointToCheck.pos.x == earTipMinus.pos.x && pointToCheck.pos.y == earTipMinus.pos.y ) )
			return false; // ignore duplicates
  Vector v0 = Vector(glm::vec2((earTipMinus.pos.x - earTip.pos.x ), (earTipMinus.pos.y - earTip.pos.y )));
	Vector v1 = Vector(glm::vec2(( earTipPlus.pos.x - earTip.pos.x ), (earTipPlus.pos.y - earTip.pos.y )));
	Vector v2 = Vector(glm::vec2(( pointToCheck.pos.x - earTip.pos.x ), (pointToCheck.pos.y - earTip.pos.y)));  

  float u = ( v1.dot( v1 ) * v2.dot( v0 ) - v1.dot( v0 ) * v2.dot( v1 ) ) 
            / ( v0.dot( v0 ) * v1.dot( v1 ) - v0.dot( v1 ) * v1.dot( v0 ) );
	float v = ( v0.dot( v0 ) * v2.dot( v1 ) - v0.dot( v1 ) * v2.dot( v0 ) ) 
            / ( v0.dot( v0 ) * v1.dot( v1 ) - v0.dot( v1 ) * v1.dot( v0 ) );  

  if( u < 0 || v < 0 || u > 1 || v > 1 || ( u + v ) > 1 ) return false;

	return true;
}

bool Polygon::isEar(Vertices* active){
  Vertices* checker = active->next->next;
  while(checker != active->previous){
    if(inTriangle( *checker, *active, *active->next, *active->previous)){
      return false;
    }
		checker = checker->next;
	}
	return true;
}



/*void Polygon::SplitToConvex(const std::vector<glm::vec2>& vert){
  ret_verts_.clear();
  if(is_convex_){//GL_TRIANGLE_FAN
    ret_verts_.emplace_back(glm::vec2(0.0f,0.0f));
    for(auto& vt : vert)
      ret_verts_.emplace_back(vt);
  }else{

  }
}*/