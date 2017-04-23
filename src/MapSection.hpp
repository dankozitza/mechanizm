// 
// MapSection.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_MAP_SECTION
#define _MECH_MAP_SECTION

#include "tools.hpp"

#include <GL/glut.h>
#include <glm/core/type_vec.hpp>
#include "Side.hpp"
#include "Block.hpp"
#include "Camera.hpp"

using namespace tools;

class MapSection {

   public:
      static const int size = 14;
      Block blocks[size][size][size];
      int sid[3];
      int s_index;
      int pid;
      int fid = -1; // file id
      bool has_sides = false;

      tools::Error (*func_gen)(MapSection&);

      MapSection();
      MapSection(int x, int y, int z, tools::Error (*func)(MapSection&));

      void save(string map_name) const;
      tools::Error load(string map_name);
		string str_sid() const;
      string get_file_name(string map_name) const;
      void generate_blocks();
      // this generates sides and sets has_sides to true
      void populate_visible_sides(vector<Side>& vsides, Camera& cam);
      void mark_invalid_block_sides();
};

#endif
