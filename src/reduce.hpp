#ifndef REDUCE_HPP_INCLUDED
#define REDUCE_HPP_INCLUDED

#include <list>
#include "defs.h"
/**
*** reduce the list of polygons down to a single polygon
**/
Polygon reduce(std::vector<Polygon*>& polygons);
/**
*** reduce the mesh down to its edges
**/
std::list<Line> detect_mesh_edges(const Mesh& mesh);
/**
*** construct the line list for a given polygon
**/
std::list<Line> make_line_list(const Polygon& poly);

#endif // REDUCE_HPP_INCLUDED
