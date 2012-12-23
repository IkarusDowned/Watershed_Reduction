#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED
#include <vector>
#include <ostream>
#include <sstream>
#include <string>

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
    size_t _p1_index;
    size_t _p2_index;
    size_t _count;
    std::string key() const
    {
        std::stringstream ss;

        if(_p1_index < _p2_index)
            ss << _p1_index << _p2_index;
        else
            ss << _p2_index << _p1_index;
        return ss.str();
    }
    bool operator==(const Line& other)
    {
        if(_p1_index == other._p1_index && _p2_index == other._p2_index)
            return true;
        if(_p1_index == other._p2_index && _p2_index == other._p1_index)
            return true;
        return false;
    }
};

void AttachPolyToMesh(Mesh& mesh, Polygon& poly);
//void AttachVertToPoly(Polygon& poly, Vertex& vertex);

#endif // DEFS_H_INCLUDED
