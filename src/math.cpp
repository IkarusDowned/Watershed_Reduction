#include "math.hpp"

#include <cmath>
#include <stdlib.h>
#include <stdio.h>

namespace vertex {

double magnitude(const Vertex& x)
{
    return ::sqrt((x._x) * (x._x) + (x._y) * (x._y));
}

}
