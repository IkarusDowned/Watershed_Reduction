#include "reduce.hpp"


#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include "math.hpp"

static const double TJUNC_ERR = 1.0;
static const double TJUNC_ERR_SQR = TJUNC_ERR * TJUNC_ERR;
static bool is_t_junction(size_t p1_index, size_t p2_index, size_t q_index)
{
    const Vertex& p1 = verticies[p1_index];
    const Vertex& p2 = verticies[p2_index];
    const Vertex& q = verticies[q_index];

    const Vertex q_minus_p1 = q - p1;
    const Vertex p2_minus_p1 = p2 - p1;
    const double q_minus_squared = vertex::square(q_minus_p1);
    const double top_dot = q_minus_p1 * p2_minus_p1;    //(q-p1) * (p2-p1)
    const double top = vertex::square(top_dot);
    //initial test to see if the point is within testing distance
    if((q_minus_squared - top / vertex::square(p2_minus_p1)) >= TJUNC_ERR_SQR )
        return false;
    const double mag_p2_p1 = vertex::magnitude(p2_minus_p1);
    double t = top_dot / mag_p2_p1;
    //second test to see if the point is on the line
    if(t < TJUNC_ERR || t > (mag_p2_p1 - TJUNC_ERR ))
        return false;
    return true;

}
static void do_tjunction_elimination(Polygon& a, Polygon&b)
{
    //iterate through a in vertex pairs, as the listed order indicates the
    //line draw order. on the last vertex, wrap around to 0th vertex
    std::vector<size_t>& a_vert_indexes = a._vert_indexes;
    std::vector<size_t>& b_vert_indexes = b._vert_indexes;
    for(size_t a_index = 0; a_index < a_vert_indexes.size(); ++a_index)
    {
        size_t p1_index,p2_index;
        p1_index = a_vert_indexes[a_index];
        p2_index = a_vert_indexes[(a_index + 1)  % a_vert_indexes.size()];
        //p1_index to p2_index is our line.
        //next, we need to loop through all of b's vertex and see if the index is the same as p1 and p2.

        for(size_t b_index = 0; b_index < b_vert_indexes.size(); ++b_index)
        {
            size_t q1_index = b_vert_indexes[b_index];
            size_t q2_index = b_vert_indexes[(b_index+1) % b_vert_indexes.size()];
            //slight loop unwind, so it does look a bit funky
            //first check if this is actually the same line
            //since winding is guaranteed CCW, we just check opposite points
            if((p1_index == q2_index && p2_index == q1_index) ||(p1_index == q1_index && p2_index == q2_index) )
                continue;

            //check p1,p2 against q1
            if(p1_index != q1_index && p2_index != q1_index)
            {
                //q1 is not on the end points, so we need to do a full t-junction test
                if(is_t_junction(p1_index,p2_index,q1_index))
                    //the vertex in between p1 and p2.
                    //this means, insert into the location in front of a_index
                    a_vert_indexes.insert(a_vert_indexes.begin()+a_index+1,q1_index);
            }
            //check p1,p2 against q2 is unneccesary, as we will do it next pass
            //check q1,q2 against p1
            if(q1_index != p1_index && q2_index != p1_index)
            {
                //p1 is not on the end points, so we need to do a full t-junction test
                if(is_t_junction(q1_index,q2_index,p1_index))
                    b_vert_indexes.insert(b_vert_indexes.begin()+b_index+1,p1_index);
            }

        }
    }
}
static inline bool collision(const BoundingBox& a, const BoundingBox& b)
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
inline static bool comp_lines(const Line& a, const Line& b)
{
    if(a.key().compare(b.key()) <= 0)
        return true;
    return false;
}
static void construct_poly(std::vector<size_t>& result_indexes, std::vector<Polygon*>& original_polygons)
{
    std::vector<Line> total_lines;
    //generate the total line list
    const size_t P = original_polygons.size();
    for(size_t i = 0; i < P; ++i)
    {
        Polygon& poly = *original_polygons[i];
        std::cout << "Poly: " << poly._parent->_level_2_id << ":" << poly._level_6_id << std::endl;
        const size_t V = poly._vert_indexes.size();
        for(size_t j = 0; j < V; ++j)
        {
            size_t p1_index = poly._vert_indexes[j];
            size_t p2_index = poly._vert_indexes[(j+1) % V];
            Line line;
            line._p1_index = p1_index;
            line._p2_index = p2_index;
            line._count = 1;
            std::cout << verticies[line._p1_index] << verticies[line._p2_index] << std::endl;
            //eliminate duplicates.
            total_lines.push_back(line);
        }

    }
    std::cout << std::endl;
    std::cout << "constructed a total of " << total_lines.size() << " lines" << std::endl;
    //std::map<std::string,Line>::iterator total_end =total_lines.end();
    //std::sort(total_lines.begin(),total_lines.end(),comp_lines);

    /*

    //next, go through the list comparing i and i+1 .
    //if they are the same, skip them
    //const size_t N = total_lines.size()-1;
    //this will hold the index of valid lines, sorted by the lines p1_index.
    std::map<size_t,Line*> valid_lines;
    std::map<std::string,Line>::iterator total_end =total_lines.end();
    for(std::map<std::string,Line>::iterator total_itr = total_lines.begin(); total_itr != total_end; ++total_itr)
    {
        if(total_itr->second._count > 2 )
        {
            std::cout << "bad line: " << verticies[total_itr->second._p1_index] << verticies[total_itr->second._p2_index] << std::endl;
            exit(1);
        }
        if(total_itr->second._count == 1)
            valid_lines[total_itr->second._p1_index] = &(total_itr->second);
    }
    //now for the fun. we go thru each line
    //adding its p1_index to the list. we reach the end when we have an iterator that stops on the p1 of the first value
    std::map<size_t,Line*>::iterator itr = valid_lines.begin();
    do {
        Line& line = *itr->second;
        std::cout << verticies[line._p1_index] << verticies[line._p2_index] << std::endl;
        result_indexes.push_back(line._p1_index);
        itr = valid_lines.find(line._p2_index);
        if(itr == valid_lines.end() )
        {
            std::cerr << "malformed valid lines list" << std::endl;
            exit(1);
        }
    }while(itr != valid_lines.begin());  //aka, we loop back
    */
    /*
    for(size_t i = 0; i < valid_lines.size(); ++i)
    {
        std::cout << verticies[valid_lines[i]._p1_index] << verticies[valid_lines[i]._p2_index] << std::endl;
    }
    */

}
Polygon reduce(std::vector<Polygon*>& polygons)
{
    //iterate through every polygon, comparing it against the other polygons
    //optimally, we would rather make a single pass through the polygons.
    //unfortunatly, there are couple cases where you do need to in fact do a per-polygon check
    //we can speed this up, however, by doing a bounding-box tests and some other tricks
    const size_t P = polygons.size();
    for(size_t c = 0; c < P; ++c)
    {
        Polygon& a = *polygons[c];
        for(size_t n = c+1; n < P; ++n)
        {
            Polygon& x = *polygons[n];

            if(collision(a._box,x._box))
            {
                //std::cout << "collision between " << a._level_6_id << "," << x._level_6_id << std::endl;
                //on a collision, we check the verts of a against x, doing t-junction testing where appropriate...
                do_tjunction_elimination(a,x);
                //and then do the same thing for x against a (as the winding orders may be different)
                //do_tjunction_elimination(x,a);
            }

        }
        std::cout << "T junction elimination on: " << a._level_6_id << std::endl;
    }

    //TODO:
    //1) line reduction based on generating a global set of lines and doing erasure
    Polygon poly;
    construct_poly(poly._vert_indexes,polygons);
    return poly;
}

