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
static void print(Polygon& polygon)
{
    std::cout << "polygon " << polygon._level_6_id << ":" << std::endl;
    for(size_t v = 0; v < polygon._vert_indexes.size();++v)
    {
        size_t from = polygon._vert_indexes[v];
        size_t to  = polygon._vert_indexes[(v+1) % polygon._vert_indexes.size()];
        std::cout << verticies[from] << "," << verticies[to] << std::endl;
    }

}
static void print(std::vector<Polygon*>& polygons)
{
    for(size_t i = 0; i < polygons.size();++i)
    {
        print(*polygons[i]);
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

    output_header(outfile);
    double before = getMillisecondsNow();
    Watersheds::iterator end = meshes.end();
    for(Watersheds::iterator itr = meshes.begin();itr != end; ++itr)
    {
        Mesh mesh =reduce(itr->second->_level_2_id,itr->second->_polygons);
        std::cout << itr->first << " " << itr->second->_level_2_id << " reduced" << std::endl;
        output_reduced_mesh(itr->first,itr->second->_level_2_id,mesh,outfile);
        destroy_mesh_data(mesh);

    }
    std::cout << "reduction took : " << getMillisecondsNow()-before << " seconds" << std::endl;
    ::destroy_mesh_data(meshes);

    return 0;
}
