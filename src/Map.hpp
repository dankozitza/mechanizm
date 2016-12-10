// 
// Map.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_MAP
#define _MECH_MAP

#include "tools.hpp"

#include <GL/glut.h>
#include "MapSection.hpp"
#include "Q.hpp"

using namespace tools;

class Map {

   public:
		Q<MapSection> ms;
      static const size_t max_sections = 8;

		Map();

		// load the proper map sections based on the new position of cam
		void update(GLfloat x, GLfloat y, GLfloat z);
};

#endif
