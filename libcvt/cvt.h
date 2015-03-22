#pragma once

#include "lloyd_euclidean_cvd.h"

namespace libcvd{
    struct SimpleMesh{
        std::vector<std::vector<double> > verts;
		std::vector<std::vector<int> > faces;
		std::vector<std::vector<double> > vertColor;
        void addVertex(double x, double y, double z){
            std::vector<double> c(3);
            c[0] = x; c[1] = y; c[2] = z;
            verts.push_back(c);
        }
        void addFace(int v1, int v2, int v3){
            std::vector<int> v(3);
            v[0] = v1; v[1] = v2; v[2] = v3;
            faces.push_back(v);
        }

        // CVT
		void addVertexColor(double x, double y, double z){
			std::vector<double> c(3);
			c[0] = x; c[1] = y; c[2] = z;
			vertColor.push_back(c);
		}
    };

    void computeCVT(SimpleMesh & m, int regions = 20, int iterations = 200)
    {
        Mesh mesh;

        // insert vertices:
        Vertex** verts = new Vertex*[m.verts.size()];
        for(int i = 0; i < (int)m.verts.size(); i++)
        {
            verts[i] = new Vertex();
            verts[i]->a = Point(m.verts[i][0], m.verts[i][1], m.verts[i][2]);
			verts[i]->index = i;
            mesh.put_vertex(verts[i]);
        }

        auto hedges = new HedgeMap();

        // insert faces:
        for(int i = 0; i < (int)m.faces.size(); i++)
            mesh.put_face(m.faces[i][0], m.faces[i][1], m.faces[i][2], verts, hedges);

		// build adjacency
		mesh.link_mesh();

		// Compute CVT
        LloydCvd cvd(&mesh);
		cvd.lloyd_euclidean_cvd(&mesh, regions, iterations);

		// DEBUG with vertex colors:
		std::map < int, std::vector<double> > vcolors;
		for (auto p : mesh.pc_){
			std::vector<double> c(3);
			c[0] = random1();c[1] = random1();c[2] = random1();
			for (auto f : p->fpc_)
				for (int i = 0; i < 3; i++)
					vcolors[f->vertex(i)->index] = c;
		}
		for (auto keyValue : vcolors){
			auto c = keyValue.second;
			m.addVertexColor(c[0], c[1], c[2]);
		}
    }
}
