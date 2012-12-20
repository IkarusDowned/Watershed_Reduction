#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#include "defs.h"
#include "watershed.hpp"
#include "reduce.hpp"
#include "perf.hpp"
using namespace std;
static char input[257] = {'\0'};
static char output[257] = {'\0'};
static int GetFileInformation()
{
    std::cout << "Please enter input filename" << std::endl;
    cin.getline(input,256,'\n');
    std::cout << "Please enter output filename" << std::endl;
    cin.getline(output,256,'\n');

    return 0;
}
static void print(std::vector<Polygon*>& polygons)
{
    for(size_t i = 0; i < polygons.size();++i)
    {
        std::cout << "polygon " << polygons[i]->_level_6_id << ":" << std::endl;
        for(size_t v = 0; v < polygons[i]->_vert_indexes.size();++v)
        {
            size_t from = polygons[i]->_vert_indexes[v];
            size_t to  = polygons[i]->_vert_indexes[(v+1) % polygons[i]->_vert_indexes.size()];
            std::cout << verticies[from] << "," << verticies[to] << std::endl;
        }
    }
}
int main(int args, char* argv[])
{


    if(args < 3)
    {
        if(GetFileInformation())
        {
            std::cerr << "Error getting file information" << std::endl;
            ::exit(1);

        }
    }
    else
    {
        size_t length = strlen(argv[1]);
        if(length >= 256) length = 256;
        strncpy(input,argv[1],length);
        length = strlen(argv[2]);
        if(length >= 256) length = 256;
        strncpy(output,argv[2],length);
    }
    ifstream infile;
    infile.open(input);
    if(!infile.good())
    {
        std::cerr << "Error opening input file: " << input << std::endl;
    }
    ofstream outfile;
    outfile.open(output);
    if(!outfile.good())
    {
        std::cerr << "Error opening output file: " << output << std::endl;
    }

    freopen("stdout.txt","w",stdout);
    Watersheds meshes;
    //construct the mesh tree
    ::construct_meshs(meshes,infile);


    Watersheds::iterator end = meshes.end();

    std::cout << "Available level 1 meshes:" << std::endl;
    for(Watersheds::iterator itr = meshes.begin();itr != end; ++itr)
    {
        std::cout << "\t" << itr->first << std::endl;
        std::cout << "\tAvailable level 2 meshes:" << std::endl;
        Level2* level2 = itr->second;
        std::cout << "\t\t" << level2->_level_2_id << std::endl;
        std::cout << "\t\tAvailable level 6 mesehes: " << std::endl;
        const size_t l = level2->_polygons.size();
        for(size_t i = 0; i < l; ++i)
        {
            Level6 *level6 = level2->_polygons[i];
            std::cout << "\t\t\t" << level6->_level_6_id << " with " << level6->_vert_indexes.size() << " verts " << std::endl;
            /*
            const size_t V = level6->_vert_indexes.size();
            for(size_t v = 0; v < V; ++v)
            {
                std::cout << "\t\t\t\t" <<  verticies[level6->_vert_indexes[v]] << std::endl;
            }
            */

        }



    }
    double before = getMillisecondsNow();
    for(Watersheds::iterator itr = meshes.begin();itr != end; ++itr)
    {
        reduce(itr->second->_polygons);
        //print(itr->second->_polygons);
    }
    std::cout << "reduction took : " << getMillisecondsNow()-before << " seconds" << std::endl;

    //todo: ITS TOO SLOW. the logic is sound, tests are working. time to optimize
    /*
    Watersheds::iterator end = meshes.end();

    for(Watersheds::iterator itr = meshes.begin();itr != end; ++itr)
        do_tjunction_elimination(*itr->second);
    std::cout << "Generate per level 2 edges" << std::endl;

    for(Watersheds::iterator itr = meshes.begin();itr != end; ++itr)
    {
        std::cout << "For mesh " << itr->first << ":" << std::endl;
        Level2* level2 = itr->second;
        std::list<Line> lines = detect_mesh_edges(*level2);

    }
    */


    ::destroy_mesh_data(meshes);

    return 0;
}
