// 
// Side.hpp
//
// Used by the mechanizm program keep track of the sides within the block grid.
// mechanizm has a vector of sides which are drawn. Most of the block grid is
// not drawn or cycled through at 60 fps.
//
// Created by Daniel Kozitza
//

#ifndef _MECH_SIDE
#define _MECH_SIDE

#include <GL/glut.h>

using namespace std;

class Side {

   public:
      int id[3];
      int index;
      bool valid = true;

      // this way we don't have to know what mapsection
      GLfloat* points[4]; // array of pointers to the 4 points within cube
      GLfloat* color;

// sides are defined by their id (layer, row, column, within the map_section),
// their side index (the index to be used for faceIndex and faceColor),
//                       pointer to the map_section?)
//
      Side() {};
      Side(int layer, int row, int column, int side_index);
};

#endif
