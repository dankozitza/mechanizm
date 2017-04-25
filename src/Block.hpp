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
		// 3 - north 5
		// 2 - up
		// 1 - south
		// 4 - east
		// 5 - west 3
      int faceIndex[6][4];
		GLfloat faceColors[6][3];

		// block types:
		//	   0 - empty
		//	   1 - solid
		int total_types = 2;
		int type = 1;

      // y, x, z ?
      int id[3];

      Block();
		Block(int identity[3]);
		Block(int layer, int row, int column);

      void set_cube(GLfloat new_cube[][3]);
      void set_faceIndex(GLfloat new_faceIndex[][4]);

      // cube transformations
      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void translate_by(GLfloat add_cube[][3]);
      void multiply_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z);
};

#endif
