// 
// Vertex.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_VERTEX
#define _MECH_VERTEX

#include <GL/glut.h>
#include <math.h>

class Vertex {

   public:

      GLfloat x, y, z;
      
      Vertex();
      Vertex(GLfloat x, GLfloat y, GLfloat z);
      Vertex(GLfloat x);
      ~Vertex();

      GLfloat dot(const Vertex & b);
      Vertex cross(const Vertex & b);
      Vertex normalize();
      void rotate_about(
            Vertex origin_point, GLfloat AX, GLfloat AY, GLfloat AZ);

      GLfloat& operator[](int index);

      Vertex operator+(const Vertex & b);
      Vertex operator+=(const Vertex & b);
      Vertex operator-(const Vertex & b);
      Vertex operator-=(const Vertex & b);
      Vertex operator*(const Vertex & b);
      Vertex operator*=(const Vertex & b);
      Vertex operator*(GLfloat b);
      Vertex operator*=(GLfloat b);
      Vertex operator/(const Vertex & b);
      Vertex operator/=(const Vertex & b);
      Vertex operator/(GLfloat b);
      Vertex operator/=(GLfloat b);
};

#endif
