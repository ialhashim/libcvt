#ifndef EDGE_H
#define EDGE_H

#include "vertex.h"
#include "face.h"

namespace A48 {

	class Hedge {
		friend class Mesh;
		friend class Face;
		friend class Edge;

		Vertex    *o_;  // origin
		Hedge     *n_;  // next in face
		Face      *f_;  // face
		Edge      *e_;  // edge

	public:
        Face* face() { return f_; }
        Vertex* org() { return o_; }
        Vertex* dst() { return mate()->org(); }
        Edge*  edge() { return e_; }
        Hedge* prev() { return (n_)? n_->next() : NULL; }
        Hedge* next() { return n_; }
		Hedge* mate();
		int getIndexMate();

		bool is_bdry();

        bool is_patch_bdry();
    private:
        void set_face(Face* f) { f_ = f; }
        void set_org(Vertex* v) { o_ = v; }
        void set_next(Hedge* h) { n_ = h; }
        void set_edge(Edge* e) { e_ = e; }

		Hedge* reuse(Vertex *v0, Vertex *v1);
	};


	class Edge  {
		friend class Mesh;
		Hedge h_[2];  // pair of half edges

	public:
		Edge(Vertex *p0, Vertex *p1);

		Hedge* hedge(int i);

		Vertex* org() { return h_[0].org(); }
		Vertex* dst() { return h_[1].org(); }

        bool is_bdry() { return (h_[0].face() == NULL || h_[1].face() == NULL); }

        bool is_patch_bdry();
    };


	/* implementation */
	Edge::Edge(Vertex *p0, Vertex *p1)
	{
		h_[0].set_org(p0); h_[1].set_org(p1);
		h_[0].set_face(NULL); h_[1].set_face(NULL);;
		h_[0].set_next(NULL); h_[1].set_next(NULL);
		h_[0].set_edge(this); h_[1].set_edge(this);
	}

	Hedge* Edge::hedge(int i)
	{
		switch (i)
		{
		case 0: return &h_[0];
		case 1: return &h_[1];
		}
		//throw Error("edge n");
	}

	Hedge* Hedge::mate()
	{
		return (this == e_->hedge(0))? e_->hedge(1): e_->hedge(0);
    }

	Hedge* Hedge::reuse(Vertex *v0, Vertex *v1)
	{
		set_org(v0); mate()->set_org(v1);
		set_next(NULL); mate()->set_next(NULL);
		set_face(NULL); mate()->set_face(NULL);
		return this;
	}

	bool Hedge::is_bdry()
	{
		return edge()->is_bdry();
	}
}

#endif
