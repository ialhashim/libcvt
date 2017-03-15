#ifndef MESH_H
#define MESH_H

#include "vertex.h"

namespace A48 {

class Mesh {
public:
    PatchContainer  pc_;   // patches
    FaceContainer   fc_;   // faces
    EdgeContainer   ec_;   // edges
    VertexContainer vc_;   // vertices

    Mesh();
    ~Mesh();

    void delete_patches();

    VertexIter verts_begin(){ return vc_.begin(); }
    VertexIter verts_end(){ return vc_.end(); }
    int num_verts() { return vc_.size(); }

    EdgeIter edges_begin() { return ec_.begin(); }
    EdgeIter edges_end() { return ec_.end(); }
    int num_edges() { return ec_.size(); }

    FaceIter faces_begin() { return fc_.begin(); }
    FaceIter faces_end() { return fc_.end(); }
    int num_faces() { return fc_.size(); }

    PatchIter patches_begin() { return pc_.begin(); }
    PatchIter patches_end() { return pc_.end(); }
    int num_patches() { return pc_.size(); }

private:
    Vertex *add_vertex();
    bool add_vertex(Vertex *v)
    { std::pair<VertexIter, bool> r = vc_.insert(v); return r.second; }
    void del_vertex(Vertex* v) { vc_.erase(v); delete(v); }

    Hedge *add_edge(Vertex *v0, Vertex *v1);
    bool add_edge(Edge *e)
    { std::pair<EdgeIter, bool> r = ec_.insert(e);  return r.second;}
    void del_edge(Edge* e) { ec_.erase(e); delete(e); }

    Face* add_face(Hedge* e0, Hedge* e1, Hedge* e2);
    Face* add_face(Hedge *e0, Hedge *e1, Hedge *e2, float v0, float v1, float v2, float v3, float v4, float v5);

    bool add_face(Face* f)
    { std::pair<FaceIter, bool> r = fc_.insert(f); return r.second;}
    void del_face(Face* f) {
        Patch *p = f->patch();

        if (p != NULL)
        {
            p->del_face_patch(f);
        }

        fc_.erase(f);
        delete(f);
    }

    Patch* add_patch();
    Patch* add_patch(int *i, int size, FaceMap* faces, int camera_index, float *box);
    bool add_patch(Patch* p)
    { std::pair<PatchIter, bool> r = pc_.insert(p); return r.second;}
    void del_patch(Patch* p) {
        for (FaceIter fiter = fc_.begin(); fiter != fc_.end(); fiter++)
        {
            Face *f = (*fiter);

            if (f->patch() == p)
            {
                f->set_patch(NULL);
            }
        }
        pc_.erase(p);
        delete(p);
    }

    Hedge* get_hedge(int i0, int i1, Vertex* verts[], HedgeMap* edges);

public:
    void put_vertex(Vertex *v) { add_vertex(v); }
    void put_face(int i0, int i1, int i2, Vertex* verts[], HedgeMap* edges);
    void put_face(int i0, int i1, int i2, float v0, float v1, float v2, float v3, float v4, float v5, Vertex* verts[], HedgeMap* hedges);
    void put_face(int i0, int i1, int i2, Vertex* verts[], HedgeMap* edges, FaceMap* faces);
    void put_face(int i0, int i1, int i2, float v0, float v1, float v2, float v3, float v4, float v5, Vertex* verts[], HedgeMap* hedges, FaceMap* faces);

	void link_mesh();

    Patch* put_patch();
    void put_patch(int *i, int size, FaceMap* faces, int camera_index, float *box);

    void remove_patch(Patch *p);
};




/* implementation */
Mesh::Mesh()
{}

Mesh::~Mesh()
{
    for (VertexIter p = vc_.begin(); p != vc_.end(); p++)
        delete *p;
    vc_.clear();
    for (EdgeIter e = ec_.begin(); e != ec_.end(); e++)
        delete *e;
    ec_.clear();
    for (FaceIter f = fc_.begin(); f != fc_.end(); f++)
        delete *f;
    fc_.clear();
    for (PatchIter p = pc_.begin(); p != pc_.end(); p++)
        delete *p;
    pc_.clear();
}

void Mesh::delete_patches()
{
    for (PatchIter p = pc_.begin(); p != pc_.end(); p++)
        delete *p;
    pc_.clear();
}

Patch* Mesh::add_patch()
{
    Patch *p = new Patch();

    p->index = pc_.size();

    add_patch(p);
    return p;
}

Patch* Mesh::add_patch(int *i, int size, FaceMap* faces, int , float *)
{
    Patch *p = new Patch();

    for (int j = 0; j < size; j++)
    {
        FaceMap::iterator fi = faces->find(i[j]);
        Face *f = (*fi).second;
        f->set_patch(p);
        p->add_face_patch(f);
    }

    p->index = pc_.size();
    add_patch(p);
    return p;
}

Face* Mesh::add_face(Hedge *e0, Hedge *e1, Hedge *e2)
{
    Face *f = new Face(e0, e1, e2);
    f->index = fc_.size();
    add_face(f);

    return f;
}

Face* Mesh::add_face(Hedge *e0, Hedge *e1, Hedge *e2, float v0, float v1, float v2, float v3, float v4, float v5)
{
    Face *f = new Face(e0, e1, e2, v0, v1, v2, v3, v4, v5);
    f->index = fc_.size();
    add_face(f);

    return f;
}

Hedge* Mesh::add_edge(Vertex *v0, Vertex *v1)
{
    Edge *e = new Edge(v0, v1);
    add_edge(e);
    return e->hedge(0);
}

Vertex* Mesh::add_vertex(void)
{
    Vertex *v = new Vertex();
    add_vertex(v);
    return v;
}

void Mesh::link_mesh()
{
    //   for (FaceIter f = fc_.begin(); f != fc_.end(); f++){
    //     printf("(%3.3f %3.3f %3.3f) - (%3.3f %3.3f %3.3f) - (%3.3f %3.3f %3.3f)\n",
    //       (*f)->vertex(0)->a.g[X], (*f)->vertex(0)->a.g[Y], (*f)->vertex(0)->a.g[Z],
    //       (*f)->vertex(1)->a.g[X], (*f)->vertex(1)->a.g[Y], (*f)->vertex(1)->a.g[Z],
    //       (*f)->vertex(2)->a.g[X], (*f)->vertex(2)->a.g[Y], (*f)->vertex(2)->a.g[Z]);
    //   }

    for (FaceIter f = fc_.begin(); f != fc_.end(); f++)
        (*f)->link_star_verts();

    for (EdgeIter e = ec_.begin(); e != ec_.end(); e++) {
		Hedge* h = nullptr;
		h = (*e)->hedge(0);
        if (h->is_bdry() && h->face() != NULL)
            h->dst()->set_star(h);
        else if (h->face() == NULL)
            h->org()->set_star(h->mate());
    }
}

Hedge* Mesh::get_hedge(int i0, int i1, Vertex* verts[], HedgeMap *hedges)
{
    bool mate = false;
    if (i0 > i1) {
        std::swap(i0, i1);
        mate = true;
    }
    Hedge* he; HedgeMap::iterator ei  = hedges->find(Ipair(i0,i1));
    if (ei == hedges->end())
        (*hedges)[Ipair(i0,i1)] = he = add_edge(verts[i0], verts[i1]);
    else
        he = (*ei).second;
    return (mate)? he->mate() : he ;
}

void Mesh::put_face(int i0, int i1, int i2, Vertex* verts[], HedgeMap* hedges)
{
    Hedge* e0 = get_hedge(i1, i2, verts, hedges);
    Hedge* e1 = get_hedge(i2, i0, verts, hedges);
    Hedge* e2 = get_hedge(i0, i1, verts, hedges);
    add_face(e0, e1, e2);
}

void Mesh::put_face(int i0, int i1, int i2, float v0, float v1, float v2, float v3, float v4, float v5, Vertex* verts[], HedgeMap* hedges)
{
    Hedge* e0 = get_hedge(i1, i2, verts, hedges);
    Hedge* e1 = get_hedge(i2, i0, verts, hedges);
    Hedge* e2 = get_hedge(i0, i1, verts, hedges);
    add_face(e0, e1, e2, v0, v1, v2, v3, v4, v5);
}

void Mesh::put_face(int i0, int i1, int i2, Vertex* verts[], HedgeMap* hedges, FaceMap* faces)
{
    Hedge* e0 = get_hedge(i1, i2, verts, hedges);
    Hedge* e1 = get_hedge(i2, i0, verts, hedges);
    Hedge* e2 = get_hedge(i0, i1, verts, hedges);

    Face *f = add_face(e0, e1, e2);
    (*faces)[(int)fc_.size() - 1] = f;
}

void Mesh::put_face(int i0, int i1, int i2, float v0, float v1, float v2, float v3, float v4, float v5, Vertex* verts[], HedgeMap* hedges, FaceMap* faces)
{
    Hedge* e0 = get_hedge(i1, i2, verts, hedges);
    Hedge* e1 = get_hedge(i2, i0, verts, hedges);
    Hedge* e2 = get_hedge(i0, i1, verts, hedges);

    Face *f = add_face(e0, e1, e2, v0, v1, v2, v3, v4, v5);
    (*faces)[(int)fc_.size() - 1] = f;
}

void Mesh::remove_patch(Patch *p)
{
    del_patch(p);
}

Patch* Mesh::put_patch()
{
    return add_patch();
}

void Mesh::put_patch(int *i, int size, FaceMap* faces, int camera_index, float *box)
{
    add_patch(i, size, faces, camera_index, box);
}

}

#endif
