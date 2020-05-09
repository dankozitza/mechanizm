// 
// MapSection.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_MAP_SECTION
#define _MECH_MAP_SECTION

#include "tools.hpp"

#include <GL/glut.h>
//#include <glm/detail/type_vec3.hpp>
#include "Block.hpp"
#include "Camera.hpp"
#include "Q.hpp"
#include "Side.hpp"

using namespace tools;

class MapSection {

   public:

      // pointer to center held within map?

      // used to tell when the edge sides in each direction have been
      // generated.
      struct EdgeSideIndicator {
         bool up;
         bool down;
         bool north;
         bool south;
         bool east;
         bool west;
      };
      EdgeSideIndicator has_edge_sides;

      static const int size = 14;
      Block blocks[size][size][size];
      int sid[3];
      int s_index;
      int pid;
      int fid = -1; // file id
      bool has_sides = false;
      Q<Side> visible_sides;

      tools::Error (*func_gen)(MapSection&);

      MapSection();
      MapSection(int x, int y, int z, tools::Error (*func)(MapSection&));

      void save(string map_name) const;
      tools::Error load(string map_name);
      string str_sid() const;
      string get_file_name(string map_name) const;
      void generate_blocks();
      // generates sides and sets has_sides to true. This must be called
      // after blocks are generated.
      void generate_visible_sides(Camera& cam);
};

#endif
