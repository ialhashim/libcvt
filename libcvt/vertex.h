#ifndef MESH_VERTEX_H
#define MESH_VERTEX_H

#include "geometry.h"
#include "edge.h"

namespace A48 {

class Vertex : public  Markable, public MxHeapable {
    friend class Mesh;
    friend class Patch;
    friend class Face;
    Hedge*      s_;    // star hedge

public:
    Point a;
    int count;
    Vector3 color;

	int index;
	int index_ply;

    Vertex() : a() {s_ = NULL; count = 0; }

    Hedge*  star_first() const { return s_; }

    void set_index(int indx) {index = indx;}
    int get_index() { return index;}

    void set_index_ply(int indx_ply) {index_ply = indx_ply;}
    int get_index_ply() { return index_ply;}

    //void calculate_normal();

    bool is_bdry();

    Hedge *star_next(Hedge *h) const;
private:
    void set_star(Hedge* h) { s_ = h; }
};

}

#endif
