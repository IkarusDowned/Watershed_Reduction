#ifndef REDUCE_HPP_INCLUDED
#define REDUCE_HPP_INCLUDED

#include <list>
#include "defs.h"
/**
*** reduce the mesh down to its edges
**/
std::list<Line> detect_mesh_edges(const Mesh& mesh);

#endif // REDUCE_HPP_INCLUDED
