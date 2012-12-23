#ifndef REDUCE_HPP_INCLUDED
#define REDUCE_HPP_INCLUDED

#include <list>
#include "defs.h"
/**
*** reduce the list of polygons down to a single polygon
**/
Mesh reduce(std::vector<Polygon*>& polygons);

#endif // REDUCE_HPP_INCLUDED
