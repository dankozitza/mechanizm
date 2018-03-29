// 
// Sphere.hpp
//
// A simple sphere. Used for selection.
//
// Created by Daniel Kozitza
//

#ifndef _MECH_SPHERE
#define _MECH_SPHERE

#include <GL/glut.h>

class Sphere {

   public:
      int index;

      GLfloat center[3];
      GLfloat color[3];
      GLfloat r;

      Sphere();
      Sphere(float x, float y, float z, float rd, float cr, float cg, float cb);
};

#endif
