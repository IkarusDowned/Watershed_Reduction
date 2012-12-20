#include "reduce.hpp"


#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

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
    const size_t b_size = b_vert_indexes.size();
    for(size_t a_index = 0; a_index < a_vert_indexes.size(); ++a_index)
    {
        size_t p1_index,p2_index;
        p1_index = a_vert_indexes[a_index];
        p2_index = a_vert_indexes[(a_index + 1)  % a_vert_indexes.size()];
        //p1_index to p2_index is our line.
        //next, we need to loop through all of b's vertex and see if the index is the same as p1 and p2.

        for(size_t b_index = 0; b_index < b_size; ++b_index)
        {
            size_t q_index = b_vert_indexes[b_index];
            //if so, skip it
            if(p1_index == q_index || p2_index == q_index)
                continue;
            //otherwise, do a full t-junction test
            if(is_t_junction(p1_index,p2_index,q_index) == false)
                continue;
            //at this point, we know we have a T-junction. we need to insert
            //the vertex in between p1 and p2.
            //this means, insert into the location in front of a_index
            a_vert_indexes.insert(a_vert_indexes.begin()+a_index+1,q_index);


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
                do_tjunction_elimination(x,a);
            }

        }
        std::cout << "T junction elimination on: " << a._level_6_id << std::endl;
    }

    //TODO:
    //1) line reduction based on generating a global set of lines and doing erasure
    return Polygon();
}

