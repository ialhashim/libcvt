# libcvt
A C++ portable header-only implementation of the Centroidal Voronoi Tessellations (CVT) on meshes.

This is a refactoring from the code found in http://w3.impa.br/~leo-ks/comp_geometry

## Usage
```
#include "libcvt/cvt.h"

libcvd::SimpleMesh mesh;
// fill the mesh with vertex and face data
libcvd::computeCVT(mesh, 30, 100);
```
