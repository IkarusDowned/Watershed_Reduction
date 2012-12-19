#ifndef WATERSHED_HPP_INCLUDED
#define WATERSHED_HPP_INCLUDED

#include <map>
#include <fstream>

#include "defs.h"
typedef std::map<unsigned short,Mesh*> Watersheds;
void construct_meshs(Watersheds&mesh_map,std::ifstream& input);
//todo move this
void do_tjunction_elimination(Mesh& mesh);
void destroy_mesh_data(Watersheds& mesh_map);

#endif // WATERSHED_HPP_INCLUDED
