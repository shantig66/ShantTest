/*
 * MeshGen.h
 *
 *  Created on: Aug 20, 2014
 *      Author: wsun2
 */

#ifndef MESHGEN_H_
#define MESHGEN_H_

#include <iostream>

#include "UnstructMesh2D.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;

namespace GEOMETRY {

class MeshGen {
public:
	MeshGen();
	std::vector<Point2D> readBdryPointsFromFile(std::string &fileName);
	std::vector<Point2D> createBdryPointsOnCircle(double r, int n);
	UnstructMesh2D generateMesh2D(std::vector<Point2D> &boundaryPoints);
	virtual ~MeshGen();
};

} /* namespace GEOMETRY */

#endif /* MESHGEN_H_ */
