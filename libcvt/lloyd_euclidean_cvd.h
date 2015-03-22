#ifndef LLOYD_EUCLIDEAN_CVD_INCLUDED // -*- C++ -*-
#define LLOYD_EUCLIDEAN_CVD_INCLUDED 

#include "mesh_geometry.h"

using namespace A48;

class ILloydCvd
{
	public:
		double alpha;
		Mesh* mesh;
		double bbox_diagonal;

		ILloydCvd(Mesh* mesh_) : mesh(mesh_), alpha(1.0)
		{
			bbox_diagonal = MeshGeometry::get_diameter( *mesh_ );
		}

		void initialize_centroids(Mesh* mesh, int k);
		virtual void update_regions(Mesh* mesh) = 0;
		virtual void update_centroids(Mesh* mesh) = 0;
	
		void lloyd_euclidean_cvd(Mesh* mesh, int regions, int iterations );
	
	protected:
		double get_energy(Patch& p, Face& f);
		Face* project_to_region(vector<Face*>& faces, Vector3 c);
		void print_centroids(Mesh* mesh);
};

class LloydCvd : public ILloydCvd
{

	public:
		LloydCvd( Mesh* mesh_ ) : ILloydCvd( mesh_ )
		{
		}

		void update_regions(Mesh* mesh);
		void update_centroids(Mesh* mesh);
};

void ILloydCvd::initialize_centroids(Mesh* mesh, int k)
{
    srand ( 0 );
	
	for(FaceIter f = mesh->faces_begin(); f != mesh->faces_end(); f++)
	{
		(*f)->calculate_center();
		(*f)->calculate_area();
		(*f)->calculate_normal();
	}
	
	for(VertexIter v = mesh->verts_begin(); v != mesh->verts_end(); v++)
	{
		(*v)->a.n.normalize();
	}
	
	vector<Face*> faces( mesh->num_faces() );
	FaceIter iter = mesh->faces_begin();
	rep(a, faces.size() )
	{
		faces[a] = (*iter);
		iter++;
	}
	
	int i = 0;
	while ( i < k )
	{
		int j = rand()%mesh->num_faces();
		Face& f = *( faces[j] );
		
		double d = (rand()%9999)/9999.;
		if ( d < ( f.area() * f.density() ) )
		{
			Patch& p = *(mesh->put_patch());
			p.set_center_face( &f );
			p.center() = f.center();
			p.normal() = f.normal();
			f.set_patch( &p );
			i++;
		}
	}
}

double ILloydCvd::get_energy(Patch& p, Face& f)
{
	double distance_scale = 2.0 / bbox_diagonal;

	double e = alpha * distance_scale * (p.center() - f.center()).norm2()
		+ (1 - alpha) * (p.normal() - f.normal()).norm2();

	return e;
}

Face* ILloydCvd::project_to_region(vector<Face*>& faces, Vector3 c )
{
	int best_face = 0;
	double min_dist = INF;

	if ( faces.size() == 0 )
	{
		cout << "Oh my god! They killed Kenny! You bastards!" << endl;
		return NULL;
	}

	urep(i,faces.size())
	{
		double dist = ( c - faces[i]->center() ).norm2();
		if ( dist < min_dist )
		{
			min_dist = dist;
			best_face = i;
		}
	}
	
	return faces[ best_face ];
}

void ILloydCvd::print_centroids(Mesh* mesh)
{
	for(PatchIter p = mesh->patches_begin(); p != mesh->patches_end(); p++)
	{
		Patch& pp = *(*p);
		cout <<	pp.center() << endl;
	}
}

void ILloydCvd::lloyd_euclidean_cvd(Mesh* mesh, int regions, int iterations )
{
	initialize_centroids(mesh, regions);
	
	for (int i = 0; i < iterations; i++)
	{
		this->update_regions(mesh);
		this->update_centroids(mesh);
		//print_centroids(mesh);
	}

	for (auto f : mesh->fc_)
	{
		f->p_->add_face_patch(f);
	}
}

void LloydCvd::update_centroids(Mesh * mesh)
{
	if (!mesh) return;

	cout << "LloydCvd::update_centroids" << endl;
	
	vector<Vector3> centers( mesh->num_patches(), Vector3(0,0,0) );
	vector<Vector3> normals( mesh->num_patches(), Vector3(0,0,0) );
	vector<double> areas( mesh->num_patches(), 0 );
	vector< vector<Face*> > patch_faces( mesh->num_patches() );
	
	for(FaceIter f = mesh->faces_begin(); f != mesh->faces_end(); f++)
	{
		auto ff = (*f);
		if (!ff) 
			continue;

		auto p = (ff->patch());
		if (!p) 
			continue;

		int pId = p->get_index();
		centers[pId] += ff->center() * (ff->area() * ff->density());
		normals[pId] += ff->normal() * (ff->area() * ff->density());
		areas[pId] += ff->area() * ff->density();
		
		patch_faces[ pId ].push_back( *f );
	}	
	
	for(PatchIter p = mesh->patches_begin(); p != mesh->patches_end(); p++)
	{
		Patch& pp = *(*p);
		int pId = pp.get_index();
		Vector3 c = centers[ pId ];
		c *= 1. / areas[pId];
		Face* fc = project_to_region( patch_faces[pId], c );
		if ( fc != NULL )
		{
			pp.center() = fc->center();
			pp.set_center_face( fc );

			normals[ pId ].normalize();
			pp.normal() = normals[ pId ];
		}

		cout << "patch " << pId << " size = " << patch_faces[pId].size() << endl;
	}
}

void LloydCvd::update_regions(Mesh* mesh)
{
	cout << "LloydCvd::update_regions" << endl;
	
	for(FaceIter f = mesh->faces_begin(); f != mesh->faces_end(); f++)
	{
		Face&ff = *(*f);
		double min_energy = INF;
		
		for(PatchIter p = mesh->patches_begin(); p != mesh->patches_end(); p++)
		{
			Patch& pp = *(*p);
		
			double energy = get_energy(pp,ff);
			if ( energy < min_energy )
			{
				min_energy = energy;
				ff.set_patch( *p );
			}
		}
	}
}


#endif
