#include "watershed.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream> //for debuging
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
//T-junction elimination test
static bool should_tjunc_test(Polygon& a, Polygon& x)
{
    return false;
}
static void detect_collisions(std::vector<Polygon*>& polygons)
{
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
                //do p2->p1 T-junction elimination
                ++collisions;
                //std::cout << "\t\t\t\tcollision between: " << p1._level_6_id << " and " << p2._level_6_id << std::endl;
            }


        }
    }
    std::cout << "\t\t\tcollisions: " << collisions << std::endl;
}
void construct_meshs(Watersheds& mesh_map,std::ifstream& input)
{
    std::cout << "Attempting to construct initial meshes" << std::endl;
    std::string line = "";
    long count = 0;
    Mesh* current_mesh;
    Polygon* current_polygon;
    std::getline(input,line);   //discard the first line
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
    for(Watersheds::iterator itr = mesh_map.begin();itr != end; ++itr)
    {
        detect_collisions(itr->second->_polygons);
    }

    std::cout << "processed: " << count << " lines of mesh data" << std::endl;

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


