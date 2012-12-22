#include "watershed.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <iostream> //for debuging

#include "reduce.hpp"

enum FIELDS {
    LEVEL_1,
    LEVEL_2,
    LEVEL_6,
    ET_X,
    ET_Y,
    ET_ORDER,
    FIELDS_SIZE
};
inline static void split(std::vector<std::string>& result, const std::string& line, char delim = ',')
{
    static std::stringstream ss;
    static std::string item;
    ss.clear();
    ss.str(line);

    while(std::getline(ss,item,delim))
        result.push_back(item);
}
inline static std::vector<std::string> split(const std::string& line,char delim = ',')
{
    static std::vector<std::string> res;
    res.clear();
    split(res,line,delim);
    return res;
}

static void load_verts_to_map(std::map<std::string,Vertex>& vert_map,std::ifstream& input)
{
    static std::string line = "";
    std::getline(input,line);   //discard first line, assume this is the column information
    static std::vector<std::string> split_line;
    size_t count = 0;
    size_t index = 0;
    while(!input.eof())
    {
        std::getline(input,line);
        split_line.clear();
        split(split_line,line);
        if(split_line.size() >= FIELDS_SIZE)
        {
            if(count % 10000 == 0) std::cout << "Loading..." << std::endl;
            std::string key = split_line[ET_X] + split_line[ET_Y];
            //only add if this vertex hasn't been seen before
            if(vert_map.find(key) == vert_map.end())
            {
                Vertex vert;
                vert._x = atol(split_line[ET_X].c_str());
                vert._y = atol(split_line[ET_Y].c_str());
                vert._index = index;
                vert_map[key] = vert;
                ++index;
            }

            ++count;
        }
    }
}

static void construct_mesh(Mesh& mesh, Polygon& poly, std::vector<std::string>& split_line,std::map<std::string,Vertex>& vert_map)
{
    mesh._level_2_id = atoi(split_line[LEVEL_2].c_str());
    poly._level_6_id = strtoul(split_line[LEVEL_6].c_str(),NULL,0);
    //construct a vertex
    if(vert_map.find(split_line[ET_X] + split_line[ET_Y]) == vert_map.end())
    {
        std::cerr << "vertex : " << split_line[ET_X] <<"," << split_line[ET_Y] << " not found!" <<std::endl;
        exit(1);
    }
    poly._vert_indexes.push_back(vert_map[split_line[ET_X] + split_line[ET_Y]]._index);
    AttachPolyToMesh(mesh,poly);

}
static void construct_box(Polygon& polygon)
{
    static long min_x;
    static long min_y;
    static long max_x;
    static long max_y;
    std::vector<size_t>& indexes = polygon._vert_indexes;
    if(indexes.size() == 0)
        return;
    min_x = max_x = verticies[indexes[0]]._x;
    min_y = max_y = verticies[indexes[0]]._y;
    for(size_t i = 1; i < indexes.size(); ++i)
    {
        //std::cout << polygon._level_6_id << "\t" << i << std::endl;
        size_t index = indexes[i];
        Vertex& v =verticies[index];
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

static void load_meshes(Watersheds& mesh_map,std::ifstream& input,std::map<std::string,Vertex>& vert_map)
{
    static std::string line = "";
    static long count = 0;
    static std::vector<std::string> split_line;
    Mesh* current_mesh = NULL;
    Polygon* current_polygon = NULL;
    std::getline(input,line);   //discard the first line, which is assumed to be the column information
    while(!input.eof())
    {
        std::getline(input,line);
        split_line.clear();
        split(split_line,line);
        if(split_line.size() >= FIELDS_SIZE)
        {
            if(count % 10000 == 0) std::cout << "Loading..." << std::endl;
            unsigned short this_level_1_id = atoi(split_line[LEVEL_1].c_str());

            if(count == 0)
            {
                //first run, setup the first mesh and polygon
                current_mesh = new Mesh();
                current_polygon = new Polygon();
                construct_mesh(*current_mesh,*current_polygon,split_line,vert_map);
                //add to the mesh_list
                mesh_map[this_level_1_id] = current_mesh;
            }
            else
            {
                unsigned short this_level_2_id = atoi(split_line[LEVEL_2].c_str());
                unsigned long this_level_6_id = strtoul(split_line[LEVEL_6].c_str(),NULL,0);
                unsigned long this_et_order = strtoul(split_line[ET_ORDER].c_str(),NULL,0);
                //new mesh, create a new mesh and start a new polygon
                if(this_level_2_id != current_mesh->_level_2_id)
                {
                    current_mesh = new Mesh();
                    //construct_box(*current_polygon);
                    current_polygon = new Polygon();
                    construct_mesh(*current_mesh,*current_polygon,split_line,vert_map);
                    mesh_map[this_level_1_id] = current_mesh;
                    //this line is processed, move on to next line
                    ++count;
                    continue;
                }
                //new polygon, same mesh. associate the polygon to the mesh,
                //add the new vertex to it
                if(this_et_order == 0)
                {
                    //construct_box(*current_polygon);
                    current_polygon = new Polygon();
                    current_polygon->_level_6_id = this_level_6_id;
                    if(vert_map.find(split_line[ET_X] + split_line[ET_Y]) == vert_map.end())
                    {
                        std::cerr << "vertex : " << split_line[ET_X] <<"," << split_line[ET_Y] << " not found!" <<std::endl;
                        exit(1);
                    }
                    current_polygon->_vert_indexes.push_back(vert_map[split_line[ET_X] + split_line[ET_Y]]._index);
                    AttachPolyToMesh(*current_mesh,*current_polygon);
                    //this line is processed, move to next line
                    ++count;
                    continue;
                }
                //same mesh, same polygon; new vertex
                if(vert_map.find(split_line[ET_X] + split_line[ET_Y]) == vert_map.end())
                {
                    std::cerr << "vertex : " << split_line[ET_X] <<"," << split_line[ET_Y] << " not found!" <<std::endl;
                    exit(1);
                }
                current_polygon->_vert_indexes.push_back(vert_map[split_line[ET_X] + split_line[ET_Y]]._index);
            }
            ++count;
        }
    }

}
inline static bool is_ccw(size_t a_index, size_t b_index, size_t c_index)
{
    const Vertex ba = verticies[b_index] - verticies[a_index];  //these are really vectors, but we can use the same class in this case
    const Vertex ca = verticies[c_index] - verticies[a_index];

    //take the cross product. if the result is negative, we have CCW winding
    if((ba._x * ca._y - ca._x * ba._y) > 0)
        return false;
    return true;
}
static void find_convex_vertex(size_t* a_index, size_t* b_index, size_t* c_index, const long min_y, const std::vector<size_t>& vertex_indexes)
{
    const size_t N = vertex_indexes.size();
    for(size_t i = 0; i < N; ++i)
    {
        Vertex& vert = verticies[vertex_indexes[i]];
        if(vert._y == min_y)
        {
            *a_index = vertex_indexes[i];
            if(i == 0)
                *b_index = vertex_indexes[vertex_indexes.size()-1];
            else
                *b_index = vertex_indexes[i-1];
            if(i == N-1)
                *c_index = vertex_indexes[0];
            else
                *c_index = vertex_indexes[i+1];
            return;
        }
    }
    std::cerr << "You have no vertex that matches the input min_y. Please check your inputs" << std::endl;
    exit(1);
}
static void rewind_ccw(std::vector<Polygon*>& polygons)
{
    const size_t N = polygons.size();
    for(size_t i = 0; i < N; ++i)
    {
        Polygon& poly = *polygons[i];
        if(poly._vert_indexes.size() < 3)
        {
            std::cerr << "Polygon is incomplete. needs at least one more vertex: " << poly._parent->_level_2_id <<":" << poly._level_6_id  << std::endl;
            exit(1);
        }
        size_t a_index;
        size_t b_index;
        size_t c_index;
        //we need to find the lowest y value vert of the polygon, and use the edge comming to that and going from it as
        //our a, b, c points to determine winding. this is because we need to deal with concave polygons.
        //to do that, we get the lowest (or highest) point.
        //in our case, a O(n) lookup matching the bounding box's min  y against each vert should work
        find_convex_vertex(&a_index,&b_index,&c_index,poly._box._min_y,poly._vert_indexes);
        if(is_ccw(a_index,b_index,c_index))
        {
            std::cout << "Poly " <<  poly._parent->_level_2_id << ":" << poly._level_6_id << " rewound to CW" << std::endl;
            std::reverse(poly._vert_indexes.begin(),poly._vert_indexes.end());
        }
    }
}
static inline bool comp_by_index(const Vertex& a, const Vertex& b)
{
    return a._index < b._index;
}

void construct_meshs(Watersheds& mesh_map,std::ifstream& input)
{

    //first pass, gather all of the vertex information
    //std::cout << "Pass one: load vertex information into map" << std::endl;
    std::map<std::string,Vertex> vert_map;
    load_verts_to_map(vert_map,input);
    //reset stream
    input.clear();
    input.seekg(0,std::ios_base::beg);
    //std::cout << "Attempting to construct initial meshes" << std::endl;
    load_meshes(mesh_map,input,vert_map);
    //std::cout << "set vertex vector" << std::endl;
    //load all the verts into the vector
    verticies.clear();  //defensive clear
    std::map<std::string,Vertex>::iterator end = vert_map.end();

    for(std::map<std::string,Vertex>::iterator itr = vert_map.begin(); itr != end; ++itr)
        verticies.push_back(itr->second);
    //sort by their index
    std::sort(verticies.begin(),verticies.end(),comp_by_index);
    //std::cout << "verticies count: " << verticies.size() << std::endl;
    //std::cout << "create boundin volumes for polygon collision testing" << std::endl;

    Watersheds::iterator ws_end = mesh_map.end();
    for(Watersheds::iterator itr = mesh_map.begin();itr !=  ws_end; ++itr)
    {
        //construct the bounding boxes for each polygon
        construct_boxes(itr->second->_polygons);
        //we also want to check the winding. if they are not wound CW, then reverse the list. why CW?
        //because the majority of the sample data was CW wound
        //at the end of this, all polygons follow the same winding.
        //this is very valuable, because that means when we do edge detection,
        //we only need to compare the end of the first line to the beginning of the second
        rewind_ccw(itr->second->_polygons);


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
            delete m->_polygons[j];
            m->_polygons[j] = NULL;
        }
        delete m;
        itr->second = NULL;

    }
}


