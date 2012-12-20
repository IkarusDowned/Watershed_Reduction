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
    Vertex operator-(const Vertex& other) const;
    Vertex operator+(const Vertex& other) const;
    inline unsigned long operator*(const Vertex& other) const
    {
        return (_x * other._x) + (_y * other._y);
    }
    friend std::ostream& operator <<(std::ostream& stream,const Vertex& v);

};

extern std::vector<Vertex> verticies;
typedef Mesh Level2;
typedef Polygon Level6;

struct Line {
    size_t _start_index;
    size_t _end_index;
    size_t _parent_poly_start_vert_index;
    unsigned long _touch_count;
    inline bool operator==(const Line& other) const
    {
        if(_start_index == other._start_index && _end_index == other._end_index)
            return true;
        if(_start_index == other._end_index && _end_index == other._start_index)
            return true;
        return false;
    }
};

void AttachPolyToMesh(Mesh& mesh, Polygon& poly);
//void AttachVertToPoly(Polygon& poly, Vertex& vertex);

#endif // DEFS_H_INCLUDED
