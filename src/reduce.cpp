#include "reduce.hpp"


#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

static bool count_not_1(const Line& l)
{
    return l._touch_count > 1 ? true : false;
}
static std::list<Line>::iterator exists(std::list<Line>& lines, const Line& line)
{
    std::list<Line>::iterator itr = lines.begin();
    std::list<Line>::iterator end = lines.end();
    for(;itr != end; ++itr)
    {
        Line& l = *itr;
        if((l._start == line._start && l._end == line._end) || (l._start == line._end && l._end == line._start))
            return itr;

    }
    return end;
}
static inline void _debug_equal(Line& line)
{
    if(line._end == line._start)
    {
        std::cout << "oops, problem with this line..." << std::endl;
        exit(1);
    }

}

std::list<Line> make_line_list(const Polygon& poly)
{
    const std::vector<Vertex*>& vertexes = poly._vertexes;
    const size_t V = vertexes.size();
    std::list<Line> lines;
    for(size_t i = 0; i < V; ++i)
    {
        Line l;
        l._touch_count = 0;
        l._end._parent = l._start._parent = NULL;
        l._start = *vertexes[i];
        //last vertex in list, wrap around
        if(i == (V-1))
            l._end = *vertexes[0];
        else
            l._end = *vertexes[i+1];
        lines.push_back(l);
    }
    return lines;
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
                    l._touch_count = 1;
                    l._end = *vertexes[0];
                    l._start = *vertexes[j];
                    l._end._parent = l._start._parent = NULL;
                    //_debug_equal(l);
                    //search and either insert or delete the line
                    std::list<Line>::iterator itr = exists(lines,l);
                    if(itr != lines.end())
                        lines.erase(itr);
                    else
                        lines.push_back(l);
                        //++(*itr)._touch_count;
                    //else
                        //lines.push_back(l);

                }
                else
                {
                    Line l;
                    l._touch_count = 1;
                    l._start = *vertexes[j];
                    l._end = *vertexes[j+1];
                    l._end._parent = l._start._parent = NULL;
                    //_debug_equal(l);
                    //serach and either insert or delete the line
                    std::list<Line>::iterator itr = exists(lines,l);
                    if(itr != lines.end())
                        lines.erase(itr);
                    else
                        lines.push_back(l);
                        //++(*itr)._touch_count;
                    //else
                        //lines.push_back(l);


                }

            }
        }


    }
    //lines.remove_if(count_not_1);
    std::list<Line>::iterator end = lines.end();
    /*
    for(std::list<Line>::iterator itr = lines.begin(); itr != end; ++itr)
    {
        Line& l = *itr;
        std::cout << "\tstart:(" << l._start._x << "," << l._start._y <<") end:(" << l._end._x << "," << l._end._y << ")" << std::endl;
    }
    */
    std::cout << "total lines remaining: " << lines.size() << std::endl;
    return lines;
}
