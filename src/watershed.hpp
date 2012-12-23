#ifndef WATERSHED_HPP_INCLUDED
#define WATERSHED_HPP_INCLUDED

#include <map>
#include <fstream>

#include "defs.h"
typedef std::map<unsigned short,Mesh*> Watersheds;
void construct_meshs(Watersheds&mesh_map,std::ifstream& input);
void destroy_mesh_data(Mesh& m);
void destroy_mesh_data(Watersheds& mesh_map);
void output_header(std::ofstream& output);
void output_reduced_mesh(unsigned short level1_id, unsigned short level2_id, Mesh& mesh,std::ofstream& output);

#endif // WATERSHED_HPP_INCLUDED
