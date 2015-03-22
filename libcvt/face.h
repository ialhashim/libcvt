#ifndef FACE_H
#define FACE_H

#include "edge.h"

namespace A48 {

	class Face {
	public:
		Patch  *p_;
		Hedge  *e_; // edge loop
		friend class Mesh;

		int index;

		Face(Hedge* e0, Hedge* e1, Hedge* e2) { reuse(e0, e1, e2); density_ = 1.0; area_ = distance_ = 0.0; p_ = nullptr; parent_ = nullptr; }
		Face(Hedge* e0, Hedge* e1, Hedge* e2, float v0, float v1, float v2, float v3, float v4, float v5) {
			reuse(e0, e1, e2);

			coordText[0][0] = v0;
			coordText[0][1] = v1;
			coordText[1][0] = v2;
			coordText[1][1] = v3;
			coordText[2][0] = v4;
			coordText[2][1] = v5;

			density_ = 1.0;
			area_ =  distance_ = 0.0;
			p_ = nullptr;
			parent_ = nullptr;
		};

        Patch* patch() { return p_;}
		Hedge* hedge(int k);
		Vertex* vertex(int k);
		Vector3 coordText[3];

		Vector3 normal_;
		double area_;
		double density_;
		Vector3 center_;

		double distance_;
		Face* parent_;

		Vector3& center() { return center_; }
		Vector3& normal() { return normal_; }
		double& area() { return area_; }
		double& density() { return density_; }
		double& distance() { return distance_; }
		Face* parent() { return parent_; }
		void set_parent( Face* p ) { parent_ = p; }

        void set_index(int indx) {index = indx;}
        int get_index() { return index;}

		void calculate_normal();
		void calculate_center();
		void calculate_area();
		double get_mean_curv();

		void set_patch(Patch* p) {p_ = p;};
		void set_hedge(int k, Hedge* h);
		void set_vertex(int k, Vertex* v);
		void set_coordText(int k, Vector3 v);

		void link_star_verts();

		Face* reuse(Hedge *e0, Hedge *e1, Hedge *e2);
	};


	/* implementation */

	/* indices of 3-ring */
#define NEXT3(k)        ((k < 2)? (k+1) : 0)
#define PREV3(k)        ((k > 0)? (k-1) : 2)

	Face* Face::reuse(Hedge *e0, Hedge *e1, Hedge *e2)
	{
		e_ = e0;
		e0->set_next(e1); e1->set_next(e2); e2->set_next(e0);
		e0->set_face(this); e1->set_face(this); e2->set_face(this);
		return this;
	}

	Hedge* Face::hedge(int k)
	{
		switch (k) {
		case 0: return e_;
		case 1: return e_->next();
		case 2: return e_->next()->next();
		}
		//throw Error("hedge index");
	}

	double Face::get_mean_curv()
	{
		Vertex* v0 = vertex(0);
		Vertex* v1 = vertex(1);
		Vertex* v2 = vertex(2);

		return 1/3. * ( v0->a.mean_curvature + v1->a.mean_curvature + v2->a.mean_curvature );
	}


	void Face::calculate_center()
	{
		Vertex* v0 = vertex(0);
		Vertex* v1 = vertex(1);
		Vertex* v2 = vertex(2);

		center_ = v0->a.g + v1->a.g + v2->a.g;
		center_ *= 1/3.;
	}

	void Face::calculate_area()
	{
		Vertex* v0 = vertex(0);
		Vertex* v1 = vertex(1);
		Vertex* v2 = vertex(2);

		Vector3 a01 = v0->a.g - v1->a.g;
		Vector3 a02 = v0->a.g - v2->a.g;

		area_ = 1/2. * (a01 ^ a02).norm();
	}

	void Face::calculate_normal()
	{
		Vertex* v0 = vertex(0);
		Vertex* v1 = vertex(1);
		Vertex* v2 = vertex(2);

		Vector3 a01 = v0->a.g - v1->a.g;
		Vector3 a02 = v0->a.g - v2->a.g;

		this->normal_ = a01 ^ a02;
		this->normal_.normalize();

		v0->a.n += normal_;  v0->count++;
		v1->a.n += normal_;  v1->count++;
		v2->a.n += normal_;  v2->count++;

		//cout << this->normal << endl;
	}

	Vertex* Face::vertex(int k)
	{
		return hedge(PREV3(k))->org();
	}

	void  Face::set_hedge(int k, Hedge* h)
	{
		Hedge *n = hedge(NEXT3(k));
		Hedge *p = hedge(PREV3(k));
		h->set_next(n);
		p->set_next(h);
		if (k == 0) e_ = h;
	}

	void Face::link_star_verts()
	{
		for (int k = 0; k < 3; k++) 
		vertex(k)->set_star(hedge(NEXT3(k)));
	}

	void  Face::set_vertex(int k, Vertex* v)
	{
		hedge(PREV3(k))->set_org(v);
	}

	void  Face::set_coordText(int k, Vector3 v)
	{
		coordText[k] = v;
	}


    int Hedge::getIndexMate()
    {
        if (this->mate()->face() != NULL)
        {
            if (this->mate()->face()->hedge(0) == this->mate())
            return 0;

            if (this->mate()->face()->hedge(1) == this->mate())
            return 1;

            if (this->mate()->face()->hedge(2) == this->mate())
            return 2;
        }

        return -1;
    }

    bool Vertex::is_bdry() { return s_->edge()->is_bdry(); }

    Hedge* Vertex::star_next(Hedge* h) const {
      if (h->face() == NULL ) return NULL; // other side of boundary
      else { Hedge *n = h->next()->mate(); return (n == s_)? NULL : n; }
    }

    bool Edge::is_patch_bdry() {
        if ( is_bdry() ) return true;
        else return ( h_[0].face()->patch() != h_[1].face()->patch() );
    }
}

#endif
