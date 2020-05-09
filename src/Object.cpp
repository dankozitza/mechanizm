// 
// Object.cpp
//
// Created by Daniel Kozitza
//

#include <cmath>
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

// default motion function
//
// it uses the get_* functions to calculate the position as a function of time
// in whatever way it can.
//
tools::Error tryhard_motion(double t, Object &self) {
   vector<GLfloat> posi, v, vi, a;
	if (self.c_qs.count("posi"))
   	posi = self.c_qs["posi"];
	else
		posi = {0.0, 0.0, 0.0};
   vi   = self.c_qs["vi"];
   if (self.get_v(v)) {
      self.set_cube(init_cube);
      self.translate_by(
            posi[0] + v[0] * t,
            posi[1] + v[1] * t,
            posi[2] + v[2] * t);
   }
   else if (self.get_a(a)) {
      //self.set_cube(init_cube);
      self.translate_by(
         vi[0] * t + a[0] * t * t,
         vi[1] * t + a[1] * t * t,
         vi[2] * t + a[2] * t * t);
//      self.translate_by(
//         posi[0] + vi[0] * t + a[0] * t * t,
//         posi[1] + vi[1] * t + a[1] * t * t,
//         posi[2] + vi[2] * t + a[2] * t * t);

   }
   return NULL;
}

void Object::initialize(string identity, Function func) {

   set_cube(init_cube);
   set_faceIndex(init_faceIndex);
   id = identity;
   last_t = 0.0;
   function = func;

   // these quantities can be initialized without worrying about them breaking
   // the physics equations. Mainly initial values.
   //c_qs["posi"] = {cube[0][0], cube[0][1], cube[0][2]};
   //c_qs["vi"].resize(3);
}

Object::Object() {
   initialize("", tryhard_motion);
}
Object::Object(string identity) {
   initialize(identity, tryhard_motion);
}
Object::Object(string identity, GLfloat mass) {
   c_qs["m"].push_back(mass);
   initialize(identity, tryhard_motion);
}
Object::Object(string identity, GLfloat mass, Function func) {
   c_qs["m"].push_back(mass);
   initialize(identity, func);
}

void Object::set_cube(GLfloat new_cube[][3]) {
   for (int i = 0; i < vertices; ++i) {
      for (int j = 0; j < 3; ++j) {
         cube[i][j] = new_cube[i][j];
      }
   }
   if (c_qs["posi"].size() != 3) {
      c_qs["posi"].resize(3);
   	c_qs["posi"][0] = cube[0][0];
   	c_qs["posi"][1] = cube[0][1];
   	c_qs["posi"][2] = cube[0][2];
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
   if (shape == "tetrahedron") {
      tetra.translate_by(x, y, z);
      return;
   }
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] += x;
      cube[i][1] += y;
      cube[i][2] += z;
   }
}

void Object::translate_by(GLfloat add_point[3]) {
   if (shape == "tetrahedron") {
      tetra.translate_by(add_point[0], add_point[1], add_point[2]);
      return;
   }
   for (int i = 0; i < vertices; ++i) {
      cube[i][0] += add_point[0];
      cube[i][1] += add_point[1];
      cube[i][2] += add_point[2];
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
   GLfloat tmp[3] = {cube[0][0], cube[0][1], cube[0][2]};

   translate_by(-1 * tmp[0], -1 * tmp[1], -1 * tmp[2]);
   multiply_vert_by(1, x,   1.0, 1.0);
   multiply_vert_by(2, x,   1.0, z);
   multiply_vert_by(3, 1.0, 1.0, z);
   multiply_vert_by(4, x,   y,   1.0);
   multiply_vert_by(5, x,   y,   z);
   multiply_vert_by(6, 1.0, y,   z);
   multiply_vert_by(7, 1.0, y,   1.0);
   translate_by(tmp[0], tmp[1], tmp[2]);
}

GLfloat Object::magnitude(vector<GLfloat> q) {
   GLfloat r = 0.0;
   for (int i = 0; i < q.size(); ++i)
      r += q[i] * q[i];
   sqrt(r);
   return r;
}

void Object::setConstQ(string name, GLfloat q) {
   vector<GLfloat> tmp;
   tmp.push_back(q);
   setConstQ(name, tmp);
}

void Object::setConstQ(string name, vector<GLfloat> q) {
   c_qs[name] = q;
}

vector<GLfloat> Object::getConstQ(string name) {
   return c_qs[name];
}

bool Object::get_m(GLfloat &m) {
   if (c_qs["m"].size() != 0) {
      m = c_qs["m"][0];
      return true;
   }
   if (c_qs["K"].size() != 0 && c_qs["v"].size() != 0) {
      m = (2 * c_qs["K"][0]) / (c_qs["v"][0] * c_qs["v"][0]);
      return true;
   }
   return false;
}

bool Object::get_v(vector<GLfloat> &v) {
   if (c_qs["v"].size() != 0) {
      v = c_qs["v"];
      return true;
   }
// this can only find the magnitude not the direction
//   if (c_qs["m"].size() != 0 && c_qs["K"].size() != 0) {
//      v = sqrt((2 * c_qs["K"][0]) / c_qs["m"][0]);
//   }
   return false;
}

bool Object::get_a(vector<GLfloat> &a) {
   if (c_qs["a"].size() != 0) {
      a = c_qs["a"];
      return true;
   }
   return false;
}

bool Object::get_K(GLfloat &K) {
   if (c_qs["K"].size() != 0) {
      K = c_qs["K"][0];
      return true;
   }
   if (c_qs["m"].size() != 0 && c_qs["v"].size() != 0) {
      K = 0.5 * c_qs["m"][0] * magnitude(c_qs["v"]) * magnitude(c_qs["v"]);
      return true;
   }
   return false;
}

bool Object::get_U(GLfloat &U) {
   if (c_qs["U"].size() != 0) {
      U = c_qs["U"][0];
      return true;
   }
   return false;
}

bool Object::get_k(GLfloat &k) {
   if (c_qs["k"].size() != 0) {
      k = c_qs["k"][0];
      return true;
   }
   return false;
}
