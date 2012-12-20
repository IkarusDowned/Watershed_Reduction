#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

#include "defs.h"
#include <cmath>
namespace vertex {

double distance_squared(const Vertex& a, const Vertex& b);
double distance(const Vertex& a, const Vertex& b);
double magnitude(const Vertex& x);
inline static double square(const Vertex& v)
{
    return static_cast<double>(v * v);
}
inline static double square(double x)
{
    return x * x;
}
}

#endif // MATH_HPP_INCLUDED
