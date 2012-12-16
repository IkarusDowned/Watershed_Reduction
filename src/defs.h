#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED
#include <vector>
struct Mesh;
struct Polygon;
struct Vertex;

struct Mesh
{
    unsigned short _level_2_id;
    std::vector<Polygon*> _polygons;
};

struct Polygon {
    Mesh* _parent;
    unsigned long _level_6_id;
    std::vector<Vertex*> _vertexes;
};

struct Vertex {
    Polygon* _parent;
    long _x;
    long _y;
    bool operator==(const Vertex& other)
    {
        bool x_equal, y_equal;
        x_equal = other._x == _x;
        y_equal = other._y == _y;
        return x_equal && y_equal;

    }
};

typedef Mesh Level2;
typedef Polygon Level6;

struct Line {
    Vertex _start;
    Vertex _end;
    unsigned long _touch_count;
};

void AttachPolyToMesh(Mesh& mesh, Polygon& poly);
void AttachVertToPoly(Polygon& poly, Vertex& vertex);

#endif // DEFS_H_INCLUDED
