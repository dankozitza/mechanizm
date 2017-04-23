// 
// Block.cpp
//
// Created by Daniel Kozitza
//

#include <cmath>
#include "Block.hpp"
#include "tools.hpp"

using namespace tools;

static GLfloat init_cube[8][3] = {
      0.0, 0.0, 0.0,
      1.0, 0.0, 0.0, // x axis
      1.0, 0.0, 1.0,
      0.0, 0.0, 1.0, // z axis
      1.0, 1.0, 0.0,
      1.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 1.0, 0.0}; // y axis

static GLfloat init_faceIndex[6][4] = {
      0, 1, 2, 3,
      1, 4, 5, 2,
      4, 7, 6, 5,
      7, 0, 3, 6,
      3, 2, 5, 6,
      7, 4, 1, 0};

Block::Block() {
   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
	id[0] = 0;
	id[1] = 0;
	id[2] = 0;
}
Block::Block(int identity[3]) {
   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
	for (int i = 0; i < 3; ++i)
		id[i] = identity[i];
}
Block::Block(int layer, int row, int column) {
   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
	id[0] = layer;
	id[1] = row;
	id[2] = column;
}

void Block::mark_invalid_sides() {
   if (sp_up != NULL) {
      //cout << "Block::mark_invalid_sides: GOT UP! sp_up->valid: " << sp_up->valid << "\n";
      //sp_up->valid = false;
      sp_up = NULL;
   }
   if (sp_down != NULL) {
//      sp_down->valid = false;
      sp_down = NULL;
   }
   if (sp_north != NULL) {
//      sp_north->valid = false;
      sp_north = NULL;
   }
   if (sp_south != NULL) {
//      sp_south->valid = false;
      sp_south = NULL;
   }
   if (sp_east != NULL) {
//      sp_east->valid = false;
      sp_east = NULL;
   }
   if (sp_west != NULL) {
//      sp_west->valid = false;
      sp_west = NULL;
   }

   //cout << "Block::mark_invalid_sides: GOT HERE!!!!!!!!!!!!!!!!!!!!\n";
}

void Block::set_cube(GLfloat new_cube[][3]) {
   for (int i = 0; i < vertices; ++i) {
      for (int j = 0; j < 3; ++j) {
         cube[i][j] = new_cube[i][j];
      }
   }
}

void Block::set_faceIndex(GLfloat new_faceIndex[][4]) {
   for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 4; ++j) {
         faceIndex[i][j] = new_faceIndex[i][j];
      }
   }
}

void Block::translate_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] += x;
      cube[i][1] += y;
      cube[i][2] += z;
   }
}

void Block::translate_by(GLfloat add_cube[][3]) {
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] += add_cube[i][0];
      cube[i][1] += add_cube[i][1];
      cube[i][2] += add_cube[i][2];
   }
}

void Block::multiply_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] *= x;
      cube[i][1] *= y;
      cube[i][2] *= z;
   }
}

void Block::multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z) {
   cube[vertex_index][0] *= x;
   cube[vertex_index][1] *= y;
   cube[vertex_index][2] *= z;
}
 

// 0 0 0
// 0 0 1
// 0 1 0
// 0 1 1
// 1 0 0
// 1 0 1
// 1 1 0
// 1 1 1
//
// // order used to build
// 0  0.0, 0.0, 0.0,
// 1  1.0, 0.0, 0.0, // x axis
// 2  1.0, 0.0, 1.0,
// 3  0.0, 0.0, 3.0, // z axis
// 4  1.0, 1.0, 0.0,
// 5  1.0, 1.0, 1.0,
// 6  0.0, 1.0, 1.0,
// 7  0.0, 2.0, 0.0}; // y axis
//
