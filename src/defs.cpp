#include "defs.h"

void AttachPolyToMesh(Mesh& mesh, Polygon& poly)
{
    mesh._polygons.push_back(&poly);
    poly._parent = &mesh;
}
void AttachVertToPoly(Polygon& poly, Vertex& vertex)
{
    poly._vertexes.push_back(&vertex);
    vertex._parent = &poly;
}
