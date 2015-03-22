#ifndef A48_H
#define A48_H

#include <cstdio>
#include <iostream>
#include <set>
#include <map>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cmath>

#include "heap.h"
#include "geometry.h"

namespace A48 {

class Mesh;
class Patch;
class Face;
class Edge;
class Hedge;
class Vertex;

class Markable;
class Error;

typedef std::set<Patch*>  PatchContainer; 
typedef PatchContainer::iterator PatchIter;

typedef std::set<Face*>  FaceContainer; 
typedef FaceContainer::iterator FaceIter;

typedef std::set<Edge*>  EdgeContainer;
typedef EdgeContainer::iterator EdgeIter;

typedef std::set<Vertex*>  VertexContainer;
typedef VertexContainer::iterator VertexIter;

class Ipair;
typedef std::map<const Ipair, A48::Hedge*> HedgeMap;

typedef std::map<const int, A48::Face*> FaceMap;

typedef std::map<const int, A48::Patch*> PatchMap;

//typedef Vec3 Vector3;

struct BoundingBox
{
  Vector3 min, max;
};

class Ipair {
  int i0; int i1;
public:
  Ipair(int p0, int p1) { i0=p0; i1=p1; }
  bool operator<(const Ipair s) const {
    return (i0 == s.i0)? (i1 < s.i1) : (i0 < s.i0);
  }
};

class Markable {
  bool     mark_;
 public:
  Markable(void) : mark_(false) {}
  void set_mark(bool m) { mark_ = m; }
  bool is_marked(void) const { return mark_; }
};

struct Point {
  Vector3     g;    // 3D position
  Vector3     n;    // normal

  Vector3     g2;    // 3D position
  Vector3     n2;    // normal

  double      mixed_area;
  double      gauss_curvature;
  double      mean_curvature;

  Vector3     t;    // texture coordinates

  Point() {g[0]=g[1]=g[2]=0; n[0]=n[1]=n[2]=0; g2[0]=g2[1]=g2[2]=0; n2[0]=n2[1]=n2[2]=0; t[0]=t[1]=t[2]=0;}
  Point(Vector3 pos) : g(pos) { n[0]=n[1]=n[2]=0; t[0]=t[1]=t[2]=0;}
  Point(double x, double y, double z) { 
    g[0]=x; g[1]=y; g[2]=z; n[0]=n[1]=n[2]=0; t[0]=t[1]=t[2]=0;;
  }
};


class Error {
 public:
  Error(char *s){ 
    std::cerr<<"AM error: "<<s<<std::endl; exit(0); 
  }
};

}

#include "face.h"
#include "patch.h"
#include "edge.h"
#include "vertex.h"
#include "mesh.h"

#endif
