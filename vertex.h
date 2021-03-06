#ifndef _VERTEX_H
#define _VERTEX_H

#include <cstdlib>
#include "vectors.h"

// ==========================================================

class Vertex {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Vertex(int i, const Vec3f &pos) : position(pos), index(i) {}
  ~Vertex() {}
  
  // =========
  // ACCESSORS
  int getIndex() const { return index; }
  double x() const { return position.x(); }
  double y() const { return position.y(); }
  double z() const { return position.z(); }
  const Vec3f& getPos() const { return position; }
  const Vec3f& getGouraudNormal() const { return gouraud_normal; }

  // =========
  // MODIFIERS
  void setPos(Vec3f v) { position = v; }
  void clearGouraudNormal() { gouraud_normal = Vec3f(0,0,0); }
  void incrGouraudNormal(Vec3f &v) { gouraud_normal += v; }
  void normalizeGouraudNormal() { gouraud_normal.Normalize(); }

private:

  // ==============
  // REPRESENTATION
  Vec3f position;
  Vec3f gouraud_normal;

  // this is the index from the original .obj file.
  // technically not part of the half-edge data structure, 
  // but we use it for hashing
  int index;  

  // NOTE: the vertices don't know anything about adjacency.  In some
  // versions of this data structure they have a pointer to one of
  // their incoming edges.  However, this data is very complicated to
  // maintain during mesh manipulation, so it has been omitted.

};

// ==========================================================

#endif

