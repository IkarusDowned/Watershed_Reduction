#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

#include "defs.h"
#include <cmath>
namespace vertex {

double distance_squared(const Vertex& a, const Vertex& b);
double distance(const Vertex& a, const Vertex& b);
double magnitude(const Vertex& x);

}

#endif // MATH_HPP_INCLUDED
