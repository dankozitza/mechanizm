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
		static const int size = 20;
		Block blocks[size][size][size];
		int sid[3];
		int s_index;

		MapSection();
      MapSection(int x, int y, int z, int section_index);

		void generate_blocks();
		void populate_visible_sides(vector<Side>& vsides, Camera& cam);
};

#endif
