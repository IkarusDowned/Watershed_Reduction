#include "watershed.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream> //for debuging

#include "reduce.hpp"
#include "math.hpp"

enum FIELDS {
    LEVEL_1,
    LEVEL_2,
    LEVEL_6,
    ET_X,
    ET_Y,
    ET_ORDER,
    FIELDS_SIZE
};
static std::vector<std::string> split(const std::string& line,char delim = ',')
{
    std::vector<std::string> res;
    std::stringstream ss(line);
    std::string item;
    while(std::getline(ss,item,delim))
        res.push_back(item);
    return res;
}

static void construct_mesh(Mesh& mesh, Polygon& poly, std::vector<std::string>& split_line)
{
    mesh._level_2_id = atoi(split_line[LEVEL_2].c_str());
    poly._level_6_id = strtoul(split_line[LEVEL_6].c_str(),NULL,0);
    //construct a vertex
    Vertex* vert = new Vertex();
    vert->_x = atol(split_line[ET_X].c_str());
    vert->_y = atol(split_line[ET_Y].c_str());
    AttachVertToPoly(poly,*vert);
    AttachPolyToMesh(mesh,poly);

}

static void construct_box(Polygon& polygon)
{
    long min_x,min_y;
    long max_x,max_y;
    std::vector<Vertex*>& verts = polygon._vertexes;
    if(verts.size() == 0)
        return;
    min_x = max_x = verts[0]->_x;
    min_y = max_y = verts[0]->_y;
    for(size_t i = 1; i < verts.size(); ++i)
    {
        Vertex& v = *verts[i];
        if(min_x > v._x) min_x = v._x;
        if(max_x < v._x) max_x = v._x;
        if(min_y > v._y) min_y = v._y;
        if(max_y < v._y) max_y = v._y;
    }
    polygon._box._max_x = max_x;
    polygon._box._max_y = max_y;
    polygon._box._min_x = min_x;
    polygon._box._min_y = min_y;
    //std::cout << "box for " << polygon._level_6_id << " created with: " << min_x << ":" << max_x << "," << min_y << ":" << max_y << std::endl;
}
inline static void construct_boxes(std::vector<Polygon*>& polygons)
{

    const size_t N = polygons.size();
    for(size_t i = 0; i < N; ++i)
        construct_box(*polygons[i]);
}
static bool collision(BoundingBox& a, BoundingBox& b)
{
    //check x values
    if(a._min_x > b._min_x && a._min_x > b._max_x)
        return false;
    if(a._max_x < b._min_x && a._max_x < b._max_x)
        return false;
    //check y values
    if(a._min_y > b._min_y && a._min_y > b._max_y)
        return false;
    if(a._max_y < b._min_y && a._max_y < b._max_y)
        return false;
    return true;
}
static const double TJUNC_ERR = 1.0;
static const double TJUNC_ERR_SQR = TJUNC_ERR * TJUNC_ERR;
inline static double square(const Vertex& v)
{
    return static_cast<double>(v * v);
}
inline static double square(double x)
{
    return x * x;
}
//checks to see if there is actually a possibly t-junction between the point and the line
static bool possible_tjunc(const Line& a, const Vertex& q)
{
    //check if the vertex is the end points. if it is, then return false
    if(a._start == q || a._end == q)
        return false;
    const Vertex& p_1 = a._start;
    const Vertex& p_2 = a._end;
    const Vertex q_minus_p_1 = q - p_1;
    const Vertex p_2_minus_p_1 = p_2 - p_1;
    const double q_minus_squared = square(q_minus_p_1);
    const double top = square(q_minus_p_1 * p_2_minus_p_1);
    const double bottom = square(p_2_minus_p_1);

    double d_squared = q_minus_squared - top / bottom;
    if(d_squared >= TJUNC_ERR_SQR)
        return false;
    double t = ((double)(q_minus_p_1 * p_2_minus_p_1)) / vertex::magnitude(p_2_minus_p_1);
    if (t < TJUNC_ERR || t > (vertex::magnitude(p_2_minus_p_1) - TJUNC_ERR))
        return false;
    return true;

}
//on tjunction
//1) modify the line to end at x
//2) createa  new line at
static void elimate_tjunction(std::list<Line>& lines,std::list<Line>::iterator& itr,Line* a, const Vertex& x)
{

    Vertex temp = a->_end;
    //split and create
    a->_end = x;
    Line l;
    l._start = x;
    l._end = temp;
    l._touch_count = 0;
    ++itr;  //advance ahead, since insert will put behind
    lines.insert(itr,l);
    --itr;  //move back. we are now at l
    --itr;  //move back, we are now at our original position;

}

static void reconstruct(Polygon& poly,std::list<Line>& lines)
{
    //clear the current vertex list. don't forget to delete the data
    std::vector<Vertex*>& verts = poly._vertexes;
    const size_t N = verts.size();
    for(size_t i = 0; i < N; ++i)
        delete verts[i];
    verts.clear();

    //for each line, we put it back into the vertex
    //with start->end order.
    //only add the first
    std::list<Line>::iterator end = lines.end();
    std::list<Line>::iterator begin = lines.begin();
    for(std::list<Line>::iterator itr = begin;itr != end;++itr)
    {
        Vertex* v = new Vertex();
        *v = (*itr)._start;
        AttachVertToPoly(poly,*v);

    }
    std::cout << "\tpoly:" << poly._level_6_id << std::endl;
    for(size_t i = 0; i < verts.size(); ++i)
    {
        const Vertex& v = *verts[i];
        std::cout << "(" << v._x << "," << v._y << ")" << std::endl;
    }


}
static void do_tjunc_elimination(Polygon* a, Polygon* x)
{
    //generate the line list for a
    std::list<Line> lines = make_line_list(*a);
    size_t initial_size = lines.size();
    //loop thru each line, checking if a tjunction test is even neccesary
    //note, we might add lines here, so you cannnot use const the end iteratr
    for(std::list<Line>::iterator itr = lines.begin(); itr != lines.end(); ++itr)
    {
        Line& line = *itr;
        std::vector<Vertex*>& x_verts = x->_vertexes;
        const size_t N = x_verts.size();
        for(size_t i = 0; i < N; ++i)
        {
            Vertex& X = *x_verts[i];
            //std::cout << a->_level_6_id << ": checking line: " << line._start << line._end << " against vertex " << X << "...";
            if(possible_tjunc(line,X))
            {
                elimate_tjunction(lines,itr,&line,X);
                std::cout << "elminating T-junction" << std::endl;
            }

        }
    }
    //recreate the vertex vector if there was a reconstruction
    if(initial_size != lines.size())
    {
        std::cout << "\t\t\treconstructing..." << std::endl;
        reconstruct(*a,lines);
    }
}


void do_tjunction_elimination(Mesh& mesh)
{

    std::vector<Polygon*> polygons = mesh._polygons;
    const size_t N = polygons.size();
    size_t collisions = 0;
    for(size_t i = 0; i < N; ++i)
    {
        Polygon& p1 = *polygons[i];
        for(size_t j = i+1; j < N; ++j)
        {
            Polygon& p2 = *polygons[j];
            if(collision(p1._box,p2._box))
            {
                //do p1->p2 T-junction elimination
                do_tjunc_elimination(&p1,&p2);
                //do p2->p1 T-junction elimination
                do_tjunc_elimination(&p2,&p1);
                ++collisions;
                std::cout << "\t\t\t\tcollision between: " << p1._level_6_id << " and " << p2._level_6_id << std::endl;
            }


        }
    }
    //std::cout << "\t\t\tcollisions: " << collisions << std::endl;

}
void construct_meshs(Watersheds& mesh_map,std::ifstream& input)
{
    std::cout << "Attempting to construct initial meshes" << std::endl;
    std::string line = "";
    long count = 0;
    Mesh* current_mesh;
    Polygon* current_polygon;
    std::getline(input,line);   //discard the first line, which is assumed to be the column information
    while(!input.eof())
    {
        std::getline(input,line);

        std::vector<std::string> split_line = split(line);
        if(split_line.size() >= FIELDS_SIZE)
        {
            if(count % 10000 == 0) std::cout << "Working..." << std::endl;
            unsigned short this_level_1_id = atoi(split_line[LEVEL_1].c_str());
            if(count == 0)
            {
                //first run, setup the first mesh and polygon
                current_mesh = new Mesh();
                current_polygon = new Polygon();
                construct_mesh(*current_mesh,*current_polygon,split_line);

                //add to the mesh_list
                mesh_map[this_level_1_id] = current_mesh;

            }
            else
            {
                unsigned short this_level_2_id = atoi(split_line[LEVEL_2].c_str());
                unsigned long this_level_6_id = strtoul(split_line[LEVEL_6].c_str(),NULL,0);
                //new mesh, create a new mesh and start a new polygon
                if(this_level_2_id != current_mesh->_level_2_id)
                {
                    current_mesh = new Mesh();
                    //construct_box(*current_polygon);
                    current_polygon = new Polygon();
                    construct_mesh(*current_mesh,*current_polygon,split_line);
                    mesh_map[this_level_1_id] = current_mesh;
                    //this line is processed, move on to next line
                    ++count;
                    continue;
                }
                //new polygon, same mesh. associate the polygon to the mesh,
                //add the new vertex to it
                if(this_level_6_id != current_polygon->_level_6_id)
                {
                    //construct_box(*current_polygon);
                    current_polygon = new Polygon();
                    current_polygon->_level_6_id = this_level_6_id;
                    Vertex* vert = new Vertex();
                    vert->_x = atol(split_line[ET_X].c_str());
                    vert->_y = atol(split_line[ET_Y].c_str());
                    AttachVertToPoly(*current_polygon,*vert);
                    current_mesh->_polygons.push_back(current_polygon);
                    //this line is processed, move to next line
                    ++count;
                    continue;
                }

                //same mesh, same polygon; new vertex
                Vertex* vert = new Vertex();
                vert->_x = atol(split_line[ET_X].c_str());
                vert->_y = atol(split_line[ET_Y].c_str());
                AttachVertToPoly(*current_polygon,*vert);

            }
            ++count;
        }

    }

    //construct the bounding boxes for each polygon
    Watersheds::iterator end = mesh_map.end();
    for(Watersheds::iterator itr = mesh_map.begin();itr != end; ++itr)
    {
        construct_boxes(itr->second->_polygons);
    }
    std::cout << "loaded: " << count << " lines of mesh data" << std::endl;

}
static void destroy_polygon_data(Polygon& poly)
{
    const size_t N = poly._vertexes.size();
    for(size_t i = 0; i < N; ++i)
    {
        delete poly._vertexes[i];
        poly._vertexes[i] = NULL;
    }
}
void destroy_mesh_data(Watersheds& mesh_map)
{
    Watersheds::iterator end = mesh_map.end();
    for(Watersheds::iterator itr = mesh_map.begin(); itr != end; ++itr)
    {
        Mesh* m = itr->second;
        const size_t J = m->_polygons.size();
        for(size_t j = 0; j < J; ++j)
        {
            destroy_polygon_data(*(m->_polygons[j]));
            delete m->_polygons[j];
            m->_polygons[j] = NULL;
        }
        delete m;
        itr->second = NULL;

    }
}


