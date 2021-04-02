// 
// ObjectMap.hpp
//
// a Map keyed by grid locations that is used to find objects near a point
// on the 3d grid. Each object on the grid updates it's location on translation.
//
// Created by Daniel Kozitza
//

#ifndef _MECH_OBJECTMAP
#define _MECH_OBJECTMAP

#include <string>
#include <vector>
#include <unordered_map>
#include "tools.hpp"
#include "Vertex.hpp"

struct oinfo {
   string gid;
   string oid;
};

unordered_map<string, vector<oinfo>>& M();

// get the section key that vertex v belongs in.
string om_get_section_key(Vertex v);
void om_add_obj(string glb_id, string obj_id, string om_key);
void om_remove_obj(string glb_id, string obj_id, string om_key);
void om_get_nearby_sections(string center, vector<string>& sections);

// draw section corner points with highlight enabled.
//void om_get_section_points(string om_key, vector<Vertex> &pnts);

string om_getJSON();

#endif
