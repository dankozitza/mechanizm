//
// Sphere.cpp
//
// Created by Daniel Kozitza
//

#include "Sphere.hpp"


Sphere::Sphere() {
   center[0] = 0.0; center[1] = 0.0; center[2] = 0.0;
   color[0] = 0.5; color[1] =  0.5; color[2] = 0.5;
   r = 1.0;
}

Sphere::Sphere(float x, float y, float z, float rd,
      float cr, float cg, float cb) {
   center[0] = x; center[1] = y; center[2] = z;
   color[0] = cr; color[1] =  cg; color[2] = cb;
   r = rd;
}
