#include "math.hpp"

#include <cmath>
#include <stdlib.h>
#include <stdio.h>

namespace vertex {

double distance_squared(const Vertex& a, const Vertex& b)
{
    if(a == b)
        return 0.0f;
    double x_part = (b._x - a._x);
    double y_part = (b._y - a._y);

    return x_part * x_part + y_part * y_part;

}


double distance(const Vertex& a, const Vertex& b)
{

    return ::sqrt(distance_squared(a,b));
}

}
