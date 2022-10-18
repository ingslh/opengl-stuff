#include "polygon.h"

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

Polygon::Polygon(const std::vector<glm::vec2>& vert)
:edge_vec_dir_(EdgeRoundDir::t_Clockwise){
  //need to insrt value to draw bezier;

  //save zhe edge vector and judge is convex or not
  vert_nums_ = static_cast<unsigned int>(vert.size());
  for(unsigned int i = 0; i < vert_nums_; i++){
    glm::vec2 step_vec;
    if( i != vert_nums_ -1)
      step_vec = vert[i] - vert[i + 1];
    else
      step_vec = vert[i] - vert[0];
    edges_.emplace_back(step_vec);
  }
  std::vector<unsigned int> unconvex_vert_list;
  std::vector<unsigned int> convex_vert_list;
  for(unsigned int i = 0; i < vert_nums_; i++){
    glm::vec3 cross_ret;
    if (i != vert_nums_ - 1) {
      cross_ret = glm::cross(glm::vec3(edges_[i].x, edges_[i].y, 0), glm::vec3(edges_[i + 1].x, edges_[i + 1].y, 0));
    }
    else {
      cross_ret = glm::cross(glm::vec3(edges_[i].x, edges_[i].y, 0), glm::vec3(edges_[0].x, edges_[0].y, 0));
    }
    if (cross_ret.z < 0){
      is_convex_ = false;
      unconvex_vert_list.emplace_back(i+1);
    }else{
      convex_vert_list.emplace_back(i+1);
    }
  }
  if(unconvex_vert_list.empty())
    is_convex_ = true;
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



/*void Polygon::SplitToConvex(const std::vector<glm::vec2>& vert){
  ret_verts_.clear();
  if(is_convex_){//GL_TRIANGLE_FAN
    ret_verts_.emplace_back(glm::vec2(0.0f,0.0f));
    for(auto& vt : vert)
      ret_verts_.emplace_back(vt);
  }else{

  }
}*/