// 
// Map.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_MAP
#define _MECH_MAP

#include <cmath>
#include "tools.hpp"

#include <GL/glut.h>
#include "MapSection.hpp"
#include "Q.hpp"

using namespace tools;

class Map {
   private:
      // check to see if the section is already loaded into the Q
      bool section_loaded(int x, int y, int z);
      // check the filesystem for an existing map section
      bool section_exists(int x, int y, int z);
      bool section_near_center(int, int, int, int, int, int, int);

   public:
		Q<MapSection> ms;
      static const size_t max_sections = 8;

		Map();

		// load the proper map sections based on the new position of cam
		void update(GLfloat x, GLfloat y, GLfloat z);
};

#endif
