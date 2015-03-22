#ifndef MESH_GEOM_H
#define MESH_GEOM_H

#include "a48.h"

using namespace A48;

class MeshGeometry
{
	public:
		static double get_diameter(Mesh& mesh);
		static vector<Face*> get_neighbours(Face* f);
		static Hedge* other_hedge( Edge* e, Hedge* h );

		static void calculate_curvatures(Mesh& mesh);
		static double calculate_mixed_area(Vertex* v);
		static double calculate_mean_curvature(Vertex* v);
		static double calculate_gauss_curvature(Vertex* v);

		static Vector3 get_hedge_vector(Hedge* h);
		static double get_opposed_angle(Hedge* m0);
};

using namespace std;

double MeshGeometry::get_diameter(Mesh& mesh)
{
	double minx=INF, maxx=-INF, miny=INF, maxy=-INF, minz=INF, maxz=-INF;

	for(VertexIter v = mesh.verts_begin(); v != mesh.verts_end(); v++)
	{
		Vector3& p = (*v)->a.g;
		minx = min( minx, (double) p.x );
		maxx = max( maxx, (double) p.x );
		miny = min( miny, (double) p.y );
		maxy = max( maxy, (double) p.y );
		minz = min( minz, (double) p.z );
		maxz = max( maxz, (double) p.z );
	}
	
	Vector3 q( maxx - minx, maxy - miny, maxz - minz );
	return q.norm();
}

Hedge* MeshGeometry::other_hedge( Edge* e, Hedge* h )
{
	if ( e->hedge(0) == h )
		return e->hedge(1);
	else
		return e->hedge(0);
}

vector<Face*> MeshGeometry::get_neighbours(Face* f)
{
	Hedge* h0 = f->hedge(0);
	Hedge* h1 = f->hedge(1);
	Hedge* h2 = f->hedge(2);
	
	Hedge* g0 = other_hedge(h0->edge(), h0);
	Hedge* g1 = other_hedge(h1->edge(), h1);
	Hedge* g2 = other_hedge(h2->edge(), h2);
	
	vector<Face*> A;
	if ( g0->face() != NULL )
		A.push_back( g0->face() );
	if ( g1->face() != NULL )
		A.push_back( g1->face() );
	if ( g2->face() != NULL )
		A.push_back( g2->face() );	
	
	return A;
}

void MeshGeometry::calculate_curvatures(Mesh& mesh)
{
	for(VertexIter v = mesh.verts_begin(); v != mesh.verts_end(); v++)
	{
		Vertex* vv = *v;
		if ( ! vv->is_bdry() )
		{
			vv->a.mixed_area = calculate_mixed_area( vv );

		//	cout << "vertex = " << vv->a.g << " - mixed area = " << vv->a.mixed_area;
			vv->a.mean_curvature = calculate_mean_curvature( vv );
		//	cout << " mean curv = " << vv->a.mean_curvature << endl;
			vv->a.gauss_curvature = calculate_gauss_curvature( vv );
		}
		else
		{
			vv->a.mean_curvature = 0;
			vv->a.gauss_curvature = 0;

		}
	}
}

Vector3 MeshGeometry::get_hedge_vector(Hedge* h)
{
	Vertex* v0 = h->org();
	Vertex* v1 = h->dst();

	return ( v1->a.g - v0->a.g );
}

double MeshGeometry::get_opposed_angle(Hedge* m0)
{
	Hedge* m1 = m0->next();
	Hedge* m2 = m1->next();
	m1 = m1->mate();

	Vector3 v1 = get_hedge_vector( m1 );
	Vector3 v2 = get_hedge_vector( m2 );

	return acos( (v1*v2) / (v1.norm()*v2.norm()) );
}

double MeshGeometry::calculate_mixed_area(Vertex* vi)
{
	double area = 0;
	for (Hedge* h = vi->star_first(); h != NULL; h = vi->star_next(h) )
	{
		Hedge* mate = h->mate();

		double aij = get_opposed_angle( h );
		double bij = get_opposed_angle( mate );

		Vector3 vv = get_hedge_vector( h );
		// Assuming Voronoi Region
		area += .125 * ( 1.0 / tan( aij ) + 1.0 / tan( bij ) ) * vv.norm2();
	}
	return area;
}

double MeshGeometry::calculate_mean_curvature(Vertex* vi)
{
	Vector3 curv = 0;
	for (Hedge* h = vi->star_first(); h != NULL; h = vi->star_next(h) )
	{
		Hedge* mate = h->mate();

		double aij = get_opposed_angle( h );
		double bij = get_opposed_angle( mate );

		Vector3 vv = get_hedge_vector( h );
		vv *= -1;
		// Assuming Voronoi Region
		curv += vv * ( .5 * ( 1.0 / tan( aij ) + 1.0 / tan( bij ) ) );
	}
	curv *= 1.0 / vi->a.mixed_area;
	Vector3 a( curv.x / curv.z, curv.y / curv.z, 0 );
//	cout << curv << " x " << a << endl;
	return .5 * curv.norm();
}

double MeshGeometry::calculate_gauss_curvature(Vertex* )
{
	return 0;
}

#endif
