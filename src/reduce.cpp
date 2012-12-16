#include "reduce.hpp"


#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

static bool count_not_0(const Line& l)
{
    return l._touch_count > 0 ? true : false;
}
static std::list<Line>::iterator exists(std::list<Line>& lines, const Line& line)
{
    std::list<Line>::iterator itr = lines.begin();
    std::list<Line>::iterator end = lines.end();
    for(;itr != end; ++itr)
    {
        Line& l = *itr;
        //we are guaranteed that the ordering of the lines is by x
        //a line exists, then, if the start point and end points are equal
        if(l._start == line._start && l._end == line._end)
            return itr;

    }
    return end;
}
static inline void reorder(Line* line)
{
    if(!line)
        return;
    //we order the line by its x coordinate.
    //the start should be less than the end

    Vertex& start = line->_start;
    Vertex& end = line->_end;
    if(start._x > end._x)
    {
        Vertex temp = line->_start;
        line->_start = line->_end;
        line->_end = temp;
    }
}
static inline void _debug_equal(Line& line)
{
    if(line._end == line._start)
    {
        std::cout << "oops, problem with this line..." << std::endl;
        exit(1);
    }

}
std::list<Line> detect_mesh_edges(const Mesh& mesh)
{
    const std::vector<Polygon*>& polygons = mesh._polygons;
    const size_t N = polygons.size();
    std::list<Line> lines;
    if(N != 0)
    {
        //go through every polygon, constructing a line
        // for each pair of vertexes.
        //the vertexes are loaded in draw order.
        //and the last vertex is guaranteed to connect to the first

        for(size_t i = 0; i < N; ++i)
        {
            const Polygon& poly = *polygons[i];
            const std::vector<Vertex*>& vertexes = poly._vertexes;
            const size_t V = vertexes.size();
            for(size_t j = 0; j < V; ++j)
            {
                //last vertex, so construct a line from the first vertex as the end point
                //and this one
                if(j == (V-1))
                {
                    Line l;
                    l._touch_count = 0;
                    l._end = *vertexes[0];
                    l._start = *vertexes[j];
                    l._end._parent = l._start._parent = NULL;
                    reorder(&l);
                    _debug_equal(l);
                    //search and either insert or delete the line
                    std::list<Line>::iterator itr = exists(lines,l);
                    if(itr != lines.end())
                        ++(*itr)._touch_count;
                    else
                        lines.push_back(l);

                }
                else
                {
                    Line l;
                    l._touch_count = 0;
                    l._start = *vertexes[j];
                    l._end = *vertexes[j+1];
                    l._end._parent = l._start._parent = NULL;
                    reorder(&l);
                    _debug_equal(l);
                    //serach and either insert or delete the line
                    std::list<Line>::iterator itr = exists(lines,l);
                    if(itr != lines.end())
                        ++(*itr)._touch_count;
                    else
                        lines.push_back(l);


                }

            }
            std::cout << "\tReduced polygon: " << poly._level_6_id << std::endl;
        }


    }
    lines.remove_if(count_not_0);
    std::list<Line>::iterator end = lines.end();
    //TODO: need T-junction elmination at the polygon creation level
    for(std::list<Line>::iterator itr = lines.begin(); itr != end; ++itr)
    {
        Line& l = *itr;
        std::cout << "\tstart:(" << l._start._x << "," << l._start._y <<") end:(" << l._end._x << "," << l._end._y << ")" << std::endl;
    }
    return lines;
}
