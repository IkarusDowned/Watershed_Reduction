#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED
#include <vector>
#include <ostream>

struct Mesh;
struct Polygon;
struct Vertex;

struct Mesh
{
    unsigned short _level_2_id;
    std::vector<Polygon*> _polygons;
};
struct BoundingBox {
    long _min_x,_max_x;
    long _min_y,_max_y;
};
struct Polygon {
    Mesh* _parent;
    unsigned long _level_6_id;
    std::vector<size_t> _vert_indexes;
    BoundingBox _box;
};

struct Vertex {
    long _x;
    long _y;
    size_t _index;
    bool operator==(const Vertex& other) const
    {
        return (_x == other._x) && (_y == other._y) ? true : false;

    }
    Vertex operator-(const Vertex& other) const;
    Vertex operator+(const Vertex& other) const;
    long operator*(const Vertex& other) const;    //dot product
    friend std::ostream& operator <<(std::ostream& stream,const Vertex& v);

};

extern std::vector<Vertex> verticies;
typedef Mesh Level2;
typedef Polygon Level6;

struct Line {
    Vertex _start;
    Vertex _end;
    unsigned long _touch_count;
};

void AttachPolyToMesh(Mesh& mesh, Polygon& poly);
//void AttachVertToPoly(Polygon& poly, Vertex& vertex);

#endif // DEFS_H_INCLUDED
