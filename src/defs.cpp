#include "defs.h"
#include <stdlib.h>

std::vector<Vertex> verticies;
void AttachPolyToMesh(Mesh& mesh, Polygon& poly)
{
    mesh._polygons.push_back(&poly);
    poly._parent = &mesh;
}


Vertex Vertex::operator-(const Vertex& other) const
{
    Vertex res = *this;
    res._x -= other._x;
    res._y -= other._y;
    return res;
}

Vertex Vertex::operator+(const Vertex& other) const
{
    Vertex res = *this;
    res._x += other._x;
    res._y += other._y;
    return res;
}


std::ostream& operator <<(std::ostream& stream,const Vertex& v)
{
    stream << "(" << v._x << "," << v._y << ")";
    return stream;
}
