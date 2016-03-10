// 
// Object.cpp
//
// Created by Daniel Kozitza
//

#include "Object.hpp"

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


Object::Object() {
   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
   ax = 10.0;
   ay = -10.0;
   az = 0.0;
   id = "";
   func_Motion = NULL;
}

Object::Object(
      string identity,
      GLfloat mass_d,
      Function function_Motion) {
   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
   ax = 10.0;
   ay = -10.0;
   az = 0.0;
   id = identity;
   c_qs["m"] = new GLfloat;
   *c_qs["m"] = mass_d;
   func_Motion = function_Motion;
}

void Object::set_cube(GLfloat new_cube[][3]) {
   for (int i = 0; i < vertices; ++i) {
      for (int j = 0; j < 3; ++j) {
         cube[i][j] = new_cube[i][j];
      }
   }
}

void Object::set_faceIndex(GLfloat new_faceIndex[][4]) {
   for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 4; ++j) {
         faceIndex[i][j] = new_faceIndex[i][j];
      }
   }
}

void Object::translate_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] += x;
      cube[i][1] += y;
      cube[i][2] += z;
   }
}

void Object::multiply_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] *= x;
      cube[i][1] *= y;
      cube[i][2] *= z;
   }
}

void Object::multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z) {
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
void Object::scale_by(GLfloat x, GLfloat y, GLfloat z) {
   multiply_vert_by(1, x,   1.0, 1.0);
   multiply_vert_by(2, x,   1.0, z);
   multiply_vert_by(3, 1.0, 1.0, z);
   multiply_vert_by(4, x,   y,   1.0);
   multiply_vert_by(5, x,   y,   z);
   multiply_vert_by(6, 1.0, y,   z);
   multiply_vert_by(7, 1.0, y,   1.0);
}
