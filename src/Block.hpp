// 
// Block.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_BLOCK
#define _MECH_BLOCK

#include <GL/glut.h>
#include "Block.hpp"
#include "Side.hpp"

using namespace std;


class Block {

   public:

      int vertices = 8;
      GLfloat cube[8][3];

		// 0 - down
		// 1 - north
		// 2 - up
		// 3 - south
		// 4 - east
		// 5 - west
      int faceIndex[6][4];
		GLfloat faceColors[6][3];

		// block types:
		//	   0 - empty
		//	   1 - solid
		int total_types = 2;
		int type = 1;

      int id[3];

      // pointers to each side (used by destructor to mark sides for removal)
      Side* sp_up    = NULL;
      Side* sp_down  = NULL;
      Side* sp_north = NULL;
      Side* sp_south = NULL;
      Side* sp_east  = NULL;
      Side* sp_west  = NULL;

      Block();
		Block(int identity[3]);
		Block(int layer, int row, int column);

      void mark_invalid_sides();

      void set_cube(GLfloat new_cube[][3]);
      void set_faceIndex(GLfloat new_faceIndex[][4]);

      // cube transformations
      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void translate_by(GLfloat add_cube[][3]);
      void multiply_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z);
};

#endif
